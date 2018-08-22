/**************************************************************************//**
 * @file
 * @brief This project demonstrates frequency generation using the
 * WTIMER module. PC10 (Expansion Header Pin 16) is configured for output compare
 * and toggles PC10 on each overflow event at a set frequency.
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

// Desired frequency in Hz
// Min: 145 Hz, Max: 9.5 MHz with default settings
#define OUT_FREQ 1000

// Default prescale value
#define WTIMER0_PRESCALE timerPrescale1

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
  wtimerCCInit.cmoa = timerOutputActionToggle;
  TIMER_InitCC(WTIMER0, 0, &wtimerCCInit);

  // Set route to Location 15 and enable
  // WTIM0_CC0 #30 is PC10 (Expansion Header Pin 16)
  WTIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC30;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Set Top value
  // Note each overflow event constitutes 1/2 the signal period
  int topValue = CMU_ClockFreqGet(cmuClock_HFPER) / (2*OUT_FREQ * (1 << WTIMER0_PRESCALE));
  TIMER_TopSet (WTIMER0, topValue);

  // Initialize and start wtimer with defined prescale
  TIMER_Init_TypeDef wtimerInit = TIMER_INIT_DEFAULT;
  wtimerInit.prescale = WTIMER0_PRESCALE;
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
