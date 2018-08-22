
/**************************************************************************//**
 * @file
 * @brief This project demonstrates the use of the RTC. The real time clock
 * is set to interrupt every 3 seconds and toggle LED1.
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
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
#include "bsp.h"
#include "em_emu.h"
#include "em_rtcc.h"
#include "em_gpio.h"
#include "em_rmu.h"
#include "em_rtc.h"

uint32_t rtccFlag;
#define Interval_S 3.0
#define LFXOFREQ 32768
#define PAUSE (Interval_S * LFXOFREQ)

/**************************************************************************//**
 * @brief RTCC interrupt service routine
 *****************************************************************************/
void RTC_IRQHandler(void)
{
  //Reset and pause counter
  RTC_Enable(false);
  RTC_CounterReset();

  // Read the interrupt source
  RTC_IntClear(RTC_IEN_COMP0);

  // Toggle LED 1
  GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);

  RTC_Enable(true);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  //Turn on the clock for the GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);

  //Enable LED1
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief RTCC initialization
 *****************************************************************************/
void rtcSetup(void)
{
  // Configure the RTC settings
  RTC_Init_TypeDef rtc = RTC_INIT_DEFAULT;

  // Enable the oscillator for the RTC
  CMU_OscillatorEnable(cmuOsc_LFXO,true,true);

  // Turn on the clock for the RTC
  CMU_ClockEnable(cmuClock_CORELE, true);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTC, true);

  // Set RTC compare value for RTC 0
  RTC_CompareSet(0,PAUSE);

  // Allow channel 0 to cause an interrupt
  RTC_IntEnable(RTC_IEN_COMP0);
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_EnableIRQ(RTC_IRQn);

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
	  EMU_EnterEM2(true);
  }

}
