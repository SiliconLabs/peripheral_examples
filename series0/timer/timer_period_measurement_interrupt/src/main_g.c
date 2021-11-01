/***************************************************************************//**
 * @file main_g.c
 * @brief This project demonstrates period measurement using input capture. A
 * periodic input signal is routed to a Compare/Capture channel, and each period
 * length is calculated from the captured edges. Connect a periodic signal to
 * the GPIO pin specified in the readme.txt input. Note maximum measurable
 * frequency is 333 kHz.
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
#define TIMER0_PRESCALE timerPrescale1

// Default clock value
#define HFPERCLK_IN_MHZ 14

// Most recent measured period in microseconds
static volatile uint32_t measuredPeriod;

// Stored edge from previous interrupt
static volatile uint32_t lastCapturedEdge;

// Number of timer overflows since last interrupt;
static volatile uint32_t overflowCount;

/**************************************************************************//**
 * @brief
 *    Interrupt handler for TIMER0
 *
 * @note
 *    The first interrupt won't produce the correct measuredPeriod value because
 *    lastCapturedEdge will be some random value.
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(TIMER0);
  TIMER_IntClear(TIMER0, flags);

  // Read the capture value from the CC register
  uint32_t current_edge = TIMER_CaptureGet(TIMER0, 0);

  // Check if the timer overflowed
  if (flags & TIMER_IF_OF) {
    overflowCount++;
  }

  // Check if a capture event occurred
  if (flags & TIMER_IF_CC0) {

    // Calculate period in microseconds, while compensating for overflows
    // Interrupt latency will affect measurements for periods below 3 microseconds (333 kHz)
    measuredPeriod = (overflowCount * (TIMER_TopGet(TIMER0) + 1)
                      - lastCapturedEdge + current_edge)
                      / (HFPERCLK_IN_MHZ * (1 << TIMER0_PRESCALE));

    // Record the capture value for the next period measurement calculation
    lastCapturedEdge = current_edge;

    // Reset the overflow count
    overflowCount = 0;
  }
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TIMER0 CC0 Location 3 (PD1) as input
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure the TIMER0 module for Capture mode and to trigger on falling edges
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Route TIMER0 CC0 to location 3 and enable CC0 route pin
  // TIM0_CC0 #3 is GPIO Pin PD1
  TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC3);

  // Initialize timer with defined prescale value
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = TIMER0_PRESCALE;
  TIMER_Init(TIMER0, &timerInit);

  // Enable TIMER0 interrupts
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0 | TIMER_IEN_OF);
  NVIC_EnableIRQ(TIMER0_IRQn);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGpio();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

