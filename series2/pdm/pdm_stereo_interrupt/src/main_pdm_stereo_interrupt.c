/***************************************************************************//**
 * @file main_pdm_stereo_interrupt.c
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
#include "em_pdm.h"

#define BUFFER_SIZE 128

// Buffer index
uint32_t bufferPtr = 0;

// Buffer for raw PDM data
uint32_t buffer[BUFFER_SIZE];

// Buffers for left/right PCM data
int16_t left[BUFFER_SIZE];
int16_t right[BUFFER_SIZE];

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
              | PDM_CFG0_FIFODVL_ONE
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

  // Enable Interrupts
  PDM->IEN = PDM_IEN_DVL;

  NVIC_ClearPendingIRQ(PDM_IRQn);
  NVIC_EnableIRQ(PDM_IRQn);
}

/***************************************************************************//**
 * @brief
 *   PDM Interrupt Handler
 ******************************************************************************/
void PDM_IRQHandler(void)
{
  uint32_t interruptFlags = PDM->IF;

  // Read data from FIFO
  if(interruptFlags & PDM_IF_DVL) {
    PDM->IF_CLR = PDM_IF_DVL;

    while(!(PDM->STATUS & PDM_STATUS_EMPTY)) {
      buffer[bufferPtr] = PDM->RXDATA;
      left[bufferPtr] = buffer[bufferPtr] & 0x0000FFFF;
      right[bufferPtr] = (buffer[bufferPtr] >> 16) & 0x0000FFFF;
      bufferPtr = (bufferPtr + 1) % BUFFER_SIZE;
    }
  }
}

/***************************************************************************//**
 * @brief
 *   Main function
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialize PDM
  initPdm();

  while(1) {
    EMU_EnterEM1();
  }
}
