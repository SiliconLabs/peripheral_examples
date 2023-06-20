/***************************************************************************//**
 * @file main_xg22_pdm_stereo_ldma.c
 * @brief This project demonstrates how to get stereo audio using PDM module and
 * LDMA.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
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
  PDM_Init_TypeDef pdmInit;

  // Set up clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PDM, true);
  CMU_ClockSelectSet(cmuClock_PDMREF, cmuSelect_HFRCODPLL); // 19 MHz

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

  // Initialize PDM registers with reset values
  PDM_Reset(PDM);

  // Configure PDM
  pdmInit.start = true;
  pdmInit.dsr = 32;
  pdmInit.gain = 5;
  pdmInit.ch0ClkPolarity = pdmCh0ClkPolarityRisingEdge;  // Normal
  pdmInit.ch1ClkPolarity = pdmCh1ClkPolarityFallingEdge; // Invert
  pdmInit.enableCh0Ch1Stereo = true;
  pdmInit.fifoValidWatermark = pdmFifoValidWatermarkFour;
  pdmInit.dataFormat = pdmDataFormatDouble16;
  pdmInit.numChannels = pdmNumberOfChannelsTwo;
  pdmInit.filterOrder = pdmFilterOrderFifth;
  pdmInit.prescaler = 5;

  // Initialize PDM peripheral
  PDM_Init(PDM, &pdmInit);
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
