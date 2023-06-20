/***************************************************************************//**
 * @file main_xg22_pdm_stereo_interrupt.c
 * @brief This project demonstrates how to get stereo audio using PDM module and
 * interrupts.
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
  pdmInit.fifoValidWatermark = pdmFifoValidWatermarkOne;
  pdmInit.dataFormat = pdmDataFormatDouble16;
  pdmInit.numChannels = pdmNumberOfChannelsTwo;
  pdmInit.filterOrder = pdmFilterOrderFifth;
  pdmInit.prescaler = 5;

  // Initialize PDM peripheral
  PDM_Init(PDM, &pdmInit);

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
