/**************************************************************************//**
 * @main_gg11_tg11.c
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
#define COMPARE_VALUE2 19000

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PC1 as output
  GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief 
 *    TIMER initialization
 *****************************************************************************/
void initWtimer(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  // Configure WTIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle;
  TIMER_InitCC(WTIMER0, 0, &timerCCInit);

  // Set route to Location 7 and enable
  // WTIM0_CC0 #7 is PC1
  WTIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC7;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Set first CCV value to trigger rising edge
  TIMER_CompareSet(WTIMER0, 0, COMPARE_VALUE1);

  // Set buffered value to next be loaded into the CCV after overflow event
  TIMER_CompareBufSet(WTIMER0, 0, COMPARE_VALUE2);

  // Initialize and start timer with default settings
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(WTIMER0, &timerInit);

  // Safely enable WTIMER0 CC0 interrupt flag
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);
  NVIC_DisableIRQ(WTIMER0_IRQn);
  TIMER_IntEnable(WTIMER0, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initializations
  initGpio();
  initWtimer();

  // Wait for rising edge
  while(!(TIMER_IntGet(WTIMER0) & TIMER_IF_CC0));
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);

  // Disable timer after falling edge
  while(!(TIMER_IntGet(WTIMER0) & TIMER_IF_CC0));
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);
  TIMER_Enable(WTIMER0, false);

  while(1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

