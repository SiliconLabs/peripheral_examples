/***************************************************************************//**
 * @file main_xg28.c
 *
 * @brief This project demonstrates polled pulse generation using TIMER
 * output compare.  A 1-second wide high pulse is driven on a GPIO pin
 * after an initial 3.5 second delay.
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

/*
 * Change this to modify the length of the delay from when the TIMER
 * starts counting to when CC0 drives the GPIO pin high.
 */
#define NUM_SEC_DELAY  3.5

// Length of the pulse high time in milliseconds
#define PULSE_WIDTH    1000

// Compare values set to generate a 1 ms pulse with default clock and prescale
static uint32_t compareValue1;
static uint32_t compareValue2;

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCmu(void)
{
  // Enable the GPIO and TIMER0 bus clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  // Do not start counter upon initialization
  timerInit.enable = false;

  // Run in one-shot mode and toggle the pin on each compare match
  timerInit.oneShot = true;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle;

  TIMER_Init(TIMER0, &timerInit);

  // Route TIMER0 CC0 output to PB4
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortB << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (4 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  /*
   * Overwrite the default of 0xFFFF in TIMER_TOP with 0xFFFFFFFF
   * because TIMER0 is 32 bits wide.
   */
  TIMER_TopSet(TIMER0, 0xFFFFFFFF);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set the first compare value
  compareValue1 = (uint32_t)(CMU_ClockFreqGet(cmuClock_TIMER0) * NUM_SEC_DELAY);

  // Set compare value to the first compare value
  TIMER_CompareSet(TIMER0, 0, compareValue1);

  /*
   * Set the second compare value since its a global variable, but
   * don't actually use it here.  It gets written to the TIMER_OC
   * register in main() after the first edge happens.
   */
  compareValue2 = compareValue1 + ((PULSE_WIDTH / 1000) * CMU_ClockFreqGet(cmuClock_TIMER0));

  // Set buffered value to next be loaded into the CCV after overflow event
  TIMER_CompareBufSet(TIMER0, 0, compareValue2);

  // Now start the TIMER
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  initCmu();

  // Configure PB4 as output
  GPIO_PinModeSet(gpioPortB, 4, gpioModePushPull, 0);

  initTIMER();

  // Wait for the first (rising) edge
  while(!(TIMER_IntGet(TIMER0) & TIMER_IF_CC0));
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);

  // Set the count for the second (falling) edge
  TIMER_CompareSet(TIMER0, 0, compareValue2);

  // Disable timer after falling edge
  while(!(TIMER_IntGet(TIMER0) & TIMER_IF_CC0));
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);
  TIMER_Enable(TIMER0, false);

  while(1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}