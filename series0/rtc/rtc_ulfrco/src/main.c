/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the use of the RTC. The timer is set to
 * interrupt after 5 seconds. The device then goes into EM3. Upon interrupt the
 * device wakes up and toggles LED's 0 and 1.
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
 
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rtc.h"
#include "em_gpio.h"

#include "bsp.h"

#define DELAY_SECONDS 5.0
#define ULFRCOFREQ    1000
#define COMPARE_TOP   (DELAY_SECONDS * ULFRCOFREQ - 1)

/**************************************************************************//**
 * @brief RTCC interrupt service routine
 *****************************************************************************/
void RTC_IRQHandler(void)
{
  //Reset counter
  RTC_CounterReset();

  //Clear interrupt flag
  RTC_IntClear(RTC_IFC_COMP0);

  // Toggle LED 0
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  //Turn on the clock for the GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);

  //Enable LED 0
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief RTCC initialization
 *****************************************************************************/
void rtcSetup(void)
{
  // Enable the oscillator for the RTC
  CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);

  // Turn on the clock for the RTC
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

  CMU_ClockEnable(cmuClock_RTC, true);

  // Set RTC compare value for RTC 0
  RTC_CompareSet(0, COMPARE_TOP);

  // Allow channel 0 to cause an interrupt
  RTC_IntEnable(RTC_IEN_COMP0);
  RTC_IntClear(RTC_IFC_COMP0);
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_EnableIRQ(RTC_IRQn);

  // Configure the RTC settings
  RTC_Init_TypeDef rtc = RTC_INIT_DEFAULT;

  // Initialise RTC with pre-defined settings
  RTC_Init(&rtc);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGPIO();

  rtcSetup();

  // Infinite loop
  while(1)
  {
    EMU_EnterEM3(true);
  }
}
