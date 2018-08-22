/**************************************************************************//**
 * @file
 * @brief This project demonstrates polled output compare to implement a
 * blocking delay. WTIMER0 is configured for output compare, and after a set
 * 3 second delay, LED0 is set to indicate the delay is complete.
 * @version 0.0.1
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_prs.h"
#include "bsp.h"

// Timer clock prescale value
#define WTIMER0_PRESCALE timerPrescale1024

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Enable LED0
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
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

  int delaycycles = (3 * CMU_ClockFreqGet(cmuClock_HFPER)) / (1 << WTIMER0_PRESCALE);
  // Set compare value for 3 second delay
  TIMER_CompareSet(WTIMER0, 0, delaycycles);

  // Initialize and start wtimer with defined prescale
  TIMER_Init_TypeDef wtimerInit = TIMER_INIT_DEFAULT;
  wtimerInit.prescale = WTIMER0_PRESCALE;
  TIMER_Init(WTIMER0, &wtimerInit);

  // Enable Interrupt, but do not enable ISR
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);
  TIMER_IntEnable(WTIMER0, TIMER_IF_CC0);
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

  // Wait for output compare to set flag
  while(!(TIMER_IntGetEnabled(WTIMER0) & TIMER_IF_CC0));

  // Indicate delay complete
  GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

  while (1)
  {
    EMU_EnterEM1();
  }
}
