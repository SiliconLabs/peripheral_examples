/***************************************************************************//**
 * @file main_s1.c
 * @brief This project demonstrates polled period measurement using input
 * capture. A periodic input signal is routed to a Compare/Capture channel, and
 * each period length is calculated from the captured edges. Connect a periodic
 * signal to the GPIO pin specified in the readme.txt for input. Note maximum
 * measurable frequency is 333 kHz.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"

// Default prescale value
#define WTIMER0_PRESCALE timerPrescale1

// Default clock value
#define HFPERCLK_IN_MHZ 19

// Most recent measured period in microseconds
static volatile uint32_t measuredPeriod;

// Stored edge from previous interrupt
static volatile uint32_t lastCapturedEdge;

// Number of timer overflows since last interrupt;
static volatile uint32_t overflowCount;

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure WTIMER0 CC0 Location 30 (PC10) as input
  GPIO_PinModeSet(gpioPortC, 10, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initWtimer(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  // Configure the WTIMER0 module for Capture mode and to trigger on falling edges
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(WTIMER0, 0, &timerCCInit);

  // Route WTIMER0 CC0 to location 30 and enable CC0 route pin
  // WTIM0_CC0 #30 is GPIO Pin PC10
  WTIMER0->ROUTELOC0 |= TIMER_ROUTELOC0_CC0LOC_LOC30;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Initialize timer with defined prescale value
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = WTIMER0_PRESCALE;
  TIMER_Init(WTIMER0, &timerInit);
}

/**************************************************************************//**
 * @brief
 *    Calculate the period of the input waveform using Capture mode on WTIMER0
 *    channel 0
 *
 * @return
 *    The period of the input waveform
 *****************************************************************************/
uint32_t calculatePeriod(void)
{
  // Read the capture value from the CC register
  uint32_t current_edge = TIMER_CaptureGet(WTIMER0, 0);

  // Check if the timer overflowed (flag gets set even if interrupt not enabled)
  if (TIMER_IntGet(WTIMER0) & TIMER_IF_OF) {
    overflowCount++;
    TIMER_IntClear(WTIMER0, TIMER_IFC_OF);
  }

  // Calculate period in microseconds, while compensating for overflows
  // Interrupt latency will affect measurements for periods below 3 microseconds (333 kHz)
  uint32_t period = (overflowCount * (TIMER_TopGet(WTIMER0) + 2)
                     - lastCapturedEdge + current_edge)
                     / (HFPERCLK_IN_MHZ * (1 << WTIMER0_PRESCALE));

  // Record the capture value for the next period measurement calculation
  lastCapturedEdge = current_edge;

  // Reset the overflow count
  overflowCount = 0;

  return period;
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initializations
  initGpio();
  initWtimer();

  while (1) {

    // Wait for a capture event to happen
    while ((WTIMER0->STATUS & TIMER_STATUS_ICV0) == 0) {}

    // Record the period into the global variable
    measuredPeriod = calculatePeriod();
  }
}

