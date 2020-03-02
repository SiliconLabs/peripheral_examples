/***************************************************************************//**
 * @file main_pdm_stereo_ldma.c
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Labs, Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "em_pdm.h"

// DMA channel used for the example
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        (1 << LDMA_CHANNEL)

// Left/right buffer size
#define BUFFER_SIZE         128

// Ping-pong buffer size
#define PP_BUFFER_SIZE      64

// Buffers for left/right PCM data
int16_t left[BUFFER_SIZE];
int16_t right[BUFFER_SIZE];

// Descriptor linked list for LDMA transfer
LDMA_Descriptor_t descLink[2];

// Buffers for ping-pong transfer
uint32_t pingBuffer[PP_BUFFER_SIZE];
uint32_t pongBuffer[PP_BUFFER_SIZE];

// Keeps track of previously written buffer
bool prevBufferPing;

/***************************************************************************//**
 * @brief
 *   Sets up PDM microphones
 ******************************************************************************/
void initPdm(void)
{
  // Set up clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PDM, true);
  CMU_ClockSelectSet(cmuClock_PDM, cmuSelect_HFRCODPLL); // 19 MHz

  // Config GPIO and pin routing
  GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 1);    // MIC_EN
  GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 0);    // PDM_CLK
  GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 0);       // PDM_DATA

  GPIO_SlewrateSet(gpioPortC, 7, 7);

  GPIO->PDMROUTE.ROUTEEN = GPIO_PDM_ROUTEEN_CLKPEN;
  GPIO->PDMROUTE.CLKROUTE = (gpioPortC << _GPIO_PDM_CLKROUTE_PORT_SHIFT)
                            | (6 << _GPIO_PDM_CLKROUTE_PIN_SHIFT);
  GPIO->PDMROUTE.DAT0ROUTE = (gpioPortC << _GPIO_PDM_DAT0ROUTE_PORT_SHIFT)
                            | (7 << _GPIO_PDM_DAT0ROUTE_PIN_SHIFT);
  GPIO->PDMROUTE.DAT1ROUTE = (gpioPortC << _GPIO_PDM_DAT1ROUTE_PORT_SHIFT)
                            | (7 << _GPIO_PDM_DAT1ROUTE_PIN_SHIFT);

  // Config PDM
  PDM->CFG0 = PDM_CFG0_STEREOMODECH01_CH01ENABLE
              | PDM_CFG0_CH0CLKPOL_NORMAL
              | PDM_CFG0_CH1CLKPOL_INVERT
              | PDM_CFG0_FIFODVL_FOUR
              | PDM_CFG0_DATAFORMAT_DOUBLE16
              | PDM_CFG0_NUMCH_TWO
              | PDM_CFG0_FORDER_FIFTH;

  PDM->CFG1 = (5 << _PDM_CFG1_PRESC_SHIFT);

  // Enable module
  PDM->EN = PDM_EN_EN;

  // Start filter
  while(PDM->SYNCBUSY != 0);
  PDM->CMD = PDM_CMD_START;

  // Config DSR/Gain
  while(PDM->SYNCBUSY != 0);
  PDM->CTRL = (3 << _PDM_CTRL_GAIN_SHIFT) | (32 << _PDM_CTRL_DSR_SHIFT);
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for ping-pong transfer
 ******************************************************************************/
void initLdma(void)
{
  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  // LDMA transfers trigger on PDM Rx Data Valid
  LDMA_TransferCfg_t periTransferTx =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_PDM_RXDATAV);

  // Link descriptors for ping-pong transfer
  descLink[0] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&PDM->RXDATA, pingBuffer,
                                     PP_BUFFER_SIZE, 1);
  descLink[1] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&PDM->RXDATA, pongBuffer,
                                     PP_BUFFER_SIZE, -1);

  // Next transfer writes to pingBuffer
  prevBufferPing = false;

  LDMA_Init(&init);

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink);
}

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler(void)
{
  uint32_t pending;

  // Read interrupt source
  pending = LDMA_IntGet();

  // Clear interrupts
  LDMA_IntClear(pending);

  // Check for LDMA error
  if(pending & LDMA_IF_ERROR) {
    // Loop here to enable the debugger to see what has happened
    while(1);
  }

  // Keep track of previously written buffer
  prevBufferPing = !prevBufferPing;
}

/***************************************************************************//**
 * @brief
 *   Main function
 ******************************************************************************/
int main(void)
{
  int i;

  // Chip errata
  CHIP_Init();

  // Initialize LDMA and PDM
  initLdma();
  initPdm();

  while(1) {
    EMU_EnterEM1();

    // After LDMA transfer completes and wakes up device from EM1,
    // convert data from ping-pong buffers to left/right PCM data
    if(prevBufferPing) {
      for(i=0; i<PP_BUFFER_SIZE; i++) {
        left[i] = pingBuffer[i] & 0x0000FFFF;
        right[i] = (pingBuffer[i] >> 16) & 0x0000FFFF;
      }
    } else {
      for(i=0; i<PP_BUFFER_SIZE; i++) {
        left[PP_BUFFER_SIZE + i] = pongBuffer[i] & 0x0000FFFF;
        right[PP_BUFFER_SIZE + i] = (pongBuffer[i] >> 16) & 0x0000FFFF;
      }
    }
  }
}
