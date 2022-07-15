/***************************************************************************//**
 * @file main.c
 *
 * @brief This project demonstrates frequency generation using the TIMER
 * module.  PA6 (check the readme.txt file for the specific expansion header
 * pin) is configured for output compare and toggles on each overflow event
 * at a set frequency with 50% duty cycle.
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
 * Desired frequency in Hz.  The default EM01GRPACLK is 19 MHz out of
 * reset, which permits a maximum frequency of 9.5 MHz.  The minimum
 * allowable frequency for this example is 1 Hz, even though TIMER0 is
 * 32 bits wide, and a minimum well below 1 Hz is possible.
 */
#define OUT_FREQ 1000

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure PA6 as output
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);
}

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
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  uint32_t timerFreq, topValue;
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  // Don't start counter on initialization
  timerInit.enable = false;

  // Configure capture/compare channel for output compare
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cofoa = timerOutputActionToggle;

  TIMER_Init(TIMER0, &timerInit);

  // Route CC0 output to PA6
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortA << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (6 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  /*
   * Set the TOP register value.  Each time the counter overflows TOP
   * is one half of the signal period.
   */
  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);
  topValue = timerFreq / (2 * OUT_FREQ) - 1;
  TIMER_TopSet (TIMER0, topValue);

  // Now start the TIMER
  TIMER_Enable(TIMER0, true);
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
  initTIMER();

  while (1) {
    EMU_EnterEM1();
  }
}
