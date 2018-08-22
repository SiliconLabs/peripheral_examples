/**************************************************************************//**
 * @file
 * @brief This project demonstrates one-shot edge output compare using the
 * TIMER module. PC10 (Expansion Header Pin 6) is configured for output
 * and after 3 seconds, CC0 sets the pin high.
 * @version 0.0.1
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_prs.h"
#include "bsp.h"

// Compare value for 3 second delay
// 41016 / (14MHz / 1024) = 3 seconds
#define COMPARE_VALUE 41016

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD1 (Expansion Header Pin 6) as output
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure TIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionSet;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set route to Location 3 and enable
  // TIM0_CC0 #3 is PD1 (Expansion Header Pin 6)
  TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC3);;

  // Set compare value for 3 second delay
  TIMER_CompareSet(TIMER0, 0, COMPARE_VALUE);

  // Initialize and start timer with defined prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1024;
  TIMER_Init(TIMER0, &timerInit);
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
  initTIMER();

  while (1)
  {
    EMU_EnterEM1();
  }
}
