/***************************************************************************//**
 * @file main.c
 *
 * @brief This project demonstrates interrupt-driven period measurement
 * using input capture.  A periodic input signal is routed to a
 * compare/capture channel, and each period length is calculated from the
 * captured edges.  Connect a periodic signal to the GPIO pin specified
 * in the readme.txt for input.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_timer.h"

// Most recent measured period in microseconds
uint32_t measuredPeriod;

// Edge pair times stored in the IRQ handler
uint32_t firstEdge;
uint32_t secondEdge;

// Was there an overflow between the edge pair
bool overflow = false;

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCMU(void)
{
  /*
   * Enable the GPIO and TIMER0 bus clocks.
   *
   * Note: On EFR32xG21 devices, calls to CMU_ClockEnable() have no
   * effect as clocks are automatically turned on/off in response to
   * on-demand requests from the peripherals.  CMU_ClockEnable() is
   * a dummy function on EFR32xG21 present for software compatibility.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure PA6 as input for TIMER0 CC0
  GPIO_PinModeSet(gpioPortA, 6, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTIMER0(void)
{
  // Initialize the timer
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  // Configure TIMER0 for input period measurement
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  timerCCInit.mode = timerCCModeCapture;
  timerCCInit.edge = timerEdgeFalling;              // Input capture on falling edges
  timerCCInit.eventCtrl = timerEventEvery2ndEdge;   // Interrupt on every other edge
  timerInit.enable = false;

  TIMER_Init(TIMER0, &timerInit);

  // Route TIMER0 CC0 input from PA6
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortA << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (6 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  /*
   * Overwrite the default of 0xFFFF in TIMER_TOP with 0xFFFFFFFF
   * because TIMER0 is 32 bits wide.
   */
  TIMER_TopSet(TIMER0, 0xFFFFFFFF);

  // Enable TIMER0 interrupts
  TIMER_IntClear(TIMER0, _TIMER_IF_MASK);
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
  NVIC_EnableIRQ(TIMER0_IRQn);

  // Enable the TIMER
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    Interrupt handler for TIMER0
 *
 * @note
 *    Because TIMER0 has a 32-bit counter and because the variables used
 *    are also 32-bit, the calculated period is limited to 2^32 - 1 ticks
 *    of the counter clock.
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  // Get the interrupt flags
  uint32_t flags = TIMER_IntGet(TIMER0);

  // Get the time at which each falling edge was captured
  firstEdge = TIMER_CaptureGet(TIMER0, 0);
  secondEdge = TIMER_CaptureGet(TIMER0, 0);

  // Was there an overflow between edges?
  if (flags & TIMER_IF_OF)
    overflow = true;

  // Clear the interrupt flags and exit
  TIMER_IntClear(TIMER0, flags);
}

/**************************************************************************//**
 * @brief
 *    Calculate the waveform period from the captured pair of edges
 *
 * @return
 *    The period of the input waveform
 *****************************************************************************/
uint32_t calculatePeriod(void)
{
  uint32_t countsBetweenEdges;

  /*
   * Calculate the frequency of TIMER0 from the bus clock.  This
   * assumes the prescaler remains at the default value of 1.
   */
  uint32_t timerClockMHz = CMU_ClockFreqGet(cmuClock_EM01GRPACLK) / 1000000;

  /*
   * Calculate the count between edges depending on whether or not
   * there was an overflow.
   */
  if (overflow)
    countsBetweenEdges = TIMER_TopGet(TIMER0) - firstEdge + 1 + secondEdge;
  else
    countsBetweenEdges = secondEdge - firstEdge;

  // Reset the overflow flag
  overflow = false;

  // Convert the count between edges to a period in microseconds
  return (countsBetweenEdges / timerClockMHz);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  initCMU();
  initGPIO();
  initTIMER0();


  while (1) {
    // Wait in EM1 for a pair of edges
    EMU_EnterEM1();

    // Record the period into the global variable
    measuredPeriod = calculatePeriod();
  }
}
