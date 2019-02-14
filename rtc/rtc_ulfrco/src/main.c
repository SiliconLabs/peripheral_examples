
/**************************************************************************//**
 * @file
 * @brief This project demonstrates the use of the RTC. The timer is set to
 * interrupt after 5 seconds. The device then goes into EM3. Upon interrupt
 * the device wakes up and toggles LED's 0 and 1.
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
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
