/**************************************************************************//**
 * @file
 * @brief This project demonstrates polled output compare to implement a
 * blocking delay. TIMER0 is configured for output compare, and after a set
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
#define TIMER0_PRESCALE timerPrescale1024

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

  int delaycycles = (3 * CMU_ClockFreqGet(cmuClock_HFPER)) / (1 << TIMER0_PRESCALE);
  // Set compare value for 3 second delay
  TIMER_CompareSet(TIMER0, 0, delaycycles);

  // Initialize and start timer with defined prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = TIMER0_PRESCALE;
  TIMER_Init(TIMER0, &timerInit);

  // Enable Interrupt, but do not enable ISR
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);
  TIMER_IntEnable(TIMER0, TIMER_IF_CC0);
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

  // Wait for output compare to set flag
  while(!(TIMER_IntGetEnabled(TIMER0) & TIMER_IF_CC0));

  // Indicate delay complete
  GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

  while (1)
  {
    EMU_EnterEM1();
  }
}
