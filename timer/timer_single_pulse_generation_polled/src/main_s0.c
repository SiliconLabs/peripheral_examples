/**************************************************************************//**
 * @main_series0.c
 * @brief This project demonstrates polled pulse generation via output compare.
 * The GPIO pin specified in the readme.txt is configured for output and
 * generates a single 1 ms pulse.
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
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"

// Compare values set to generate a 1 ms pulse with default clock and prescale
#define COMPARE_VALUE1 65535
#define COMPARE_VALUE2 14000

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD6 as output
  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Configure TIMER1 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Set route to Location 4 and enable
  // TIM1_CC0 #4 is PD6
  TIMER1->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC4);

  // Set first CCV value to trigger rising edge
  TIMER_CompareSet(TIMER1, 0, COMPARE_VALUE1);

  // Set buffered value to next be loaded into the CCV after overflow event
  TIMER_CompareBufSet(TIMER1, 0, COMPARE_VALUE2);

  // Initialize and start timer with default settings
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(TIMER1, &timerInit);

  // Safely enable TIMER1 CC0 interrupt flag
  TIMER_IntClear(TIMER1, TIMER_IFC_CC0);
  NVIC_DisableIRQ(TIMER1_IRQn);
  TIMER_IntEnable(TIMER1, TIMER_IEN_CC0);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGpio();
  initTimer();

  // Wait for rising edge
  while(!(TIMER_IntGet(TIMER1) & TIMER_IF_CC0));
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);

  // Disable timer after falling edge
  while(!(TIMER_IntGet(TIMER1) & TIMER_IF_CC0));
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);
  TIMER_Enable(TIMER1, false);

  while(1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

