/**************************************************************************//**
 * @file
 * @brief This project demonstrates one-shot edge output compare using the
 * WTIMER module. PC10 (Expansion Header Pin 16) is configured for output
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
// 55664 / (19MHz / 1024) = 3 seconds
#define COMPARE_VALUE 55664

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PC10 (Expansion Header Pin 16) as output
  GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief WTIMER initialization
 *****************************************************************************/
void initWTIMER(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  // Configure WTIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef wtimerCCInit = TIMER_INITCC_DEFAULT;
  wtimerCCInit.mode = timerCCModeCompare;
  wtimerCCInit.cmoa = timerOutputActionSet;
  TIMER_InitCC(WTIMER0, 0, &wtimerCCInit);

  // Set route to Location 15 and enable
  // WTIM0_CC0 #30 is PC10 (Expansion Header Pin 16)
  WTIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC30;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Set compare value for 3 second delay
  TIMER_CompareSet(WTIMER0, 0, COMPARE_VALUE);

  // Initialize and start wtimer with defined prescale
  TIMER_Init_TypeDef wtimerInit = TIMER_INIT_DEFAULT;
  wtimerInit.prescale = timerPrescale1024;
  TIMER_Init(WTIMER0, &wtimerInit);
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
  initWTIMER();

  while (1)
  {
    EMU_EnterEM1();
  }
}
