/***************************************************************************//**
 * @file main.c
 *
 * @brief This project demonstrates interrupt-driven input capture using
 * the TIMER module.  The count at which each push button 0 press is
 * detected is captured by the input capture logic and saved in a buffer
 * during the interrupt handler.
 *
 * The example also has instructions on using polled mode for input capture.
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
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"

#include "bsp.h"

// Needs to be volatile so compiler does not optimize it away
#define BUFFER_SIZE 8
static volatile uint32_t buffer[BUFFER_SIZE];

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
  // Configure PB0 pin as input with the pull-up and filter enabled.
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN,
                  gpioModeInputPullFilter, 1);

  // Route the PB0 pin to TIMER0 capture/compare channel 0 and enable
  GPIO->TIMERROUTE[0].CC0ROUTE = (BSP_GPIO_PB0_PORT << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                               | (BSP_GPIO_PB0_PIN << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *
 * @note
 *    Prescaling the TIMER clock may or may not be necessary in a given
 *    application.  Here, prescaling the clock by 1024 (each TIMER tick
 *    is 1024 HFPERLKs) makes it easier to see the difference between
 *    successive capture values.
 *****************************************************************************/
void initTIMER0(void)
{
  // Initialize TIMER0
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1024;
  timerInit.enable = false;               // Don't start the timer yet

  // Configure TIMER0 capture/compare channel 0 for input capture
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;

  TIMER_Init(TIMER0, &timerInit);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  /**************************************************************************//**
   * POLLED MODE INSTRUCTIONS
   * -------------------------
   *
   * To use this example in polled mode instead of interrupt mode -
   *
   * 1. Remove TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
   * 2. Remove NVIC_EnableIRQ(TIMER0_IRQn);
   *
   *****************************************************************************/

  // Enable interrupts on capture/compare channel 0
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
  NVIC_EnableIRQ(TIMER0_IRQn);

  // Now start the TIMER
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * POLLED MODE INSTRUCTIONS
 * -------------------------
 *
 * To use this example in polled mode instead of interrupt mode, remove
 * the TIMER0_IRQHandler(void) function.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @brief
 *    TIMER 0 handler
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  static uint32_t i = 0;

  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(TIMER0);
  TIMER_IntClear(TIMER0, flags);

  // Check for capture event on channel 0
  if (flags & TIMER_IF_CC0) {

    // Record input capture value
    buffer[i] = TIMER_CaptureGet(TIMER0, 0);

    // Increment index and have it wrap around
    i = (i + 1) % BUFFER_SIZE;
  }
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  initCMU();
  initGPIO();
  initTIMER0();

  /**************************************************************************//**
  * POLLED MODE INSTRUCTIONS
  * -------------------------
  *
  * To use this example in polled mode instead of interrupt mode,
  * insert...
  *
  * uint32_t i = 0;
  *
  * ...here, before the while loop that follows.
  *
  *****************************************************************************/

  while (1) {
  /**************************************************************************//**
   * POLLED MODE INSTRUCTIONS
   * -------------------------
   *
   * To use this example in polled mode instead of interrupt mode.
   *
   * 1. Remove EMU_EnterEM1();
   *
   * 2. Replace with -
   *
   *    // Wait for a capture event to happen
   *    while (TIMER0->STATUS & TIMER_STATUS_ICFEMPTY0);
   *
   *    // Record input capture value
   *    buffer[i] = TIMER_CaptureGet(TIMER0, 0);
   *
   *    // Increment index and have it wrap around
   *    i = (i + 1) % BUFFER_SIZE;
   *
   *****************************************************************************/
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}
