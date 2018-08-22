/**************************************************************************//**
 * @file
 * @brief This project demonstrates pulse width modulation using the LETIMER.
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
#include "em_letimer.h"
#include "bsp.h"

// Desired frequency in Hz
#define OUT_FREQ 1

// Duty cycle percentage
#define DUTY_CYCLE 30

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD6 (Expansion Header 16) as output
  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initLETIMER(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Enable clock to the LE modules interface
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Select LFXO for the LETIMER
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LETIMER0, true);

  // Reload COMP0 on underflow, pulse output, and run in repeat mode
  letimerInit.comp0Top = true;
  letimerInit.ufoa0 = letimerUFOAPwm;
  letimerInit.repMode = letimerRepeatFree;

  // Need REP0 != 0 to run PWM
  LETIMER_RepeatSet(LETIMER0, 0, 1);

  // Set COMP0 to desired PWM frequency
  LETIMER_CompareSet(LETIMER0, 0,
       CMU_ClockFreqGet(cmuClock_LETIMER0) / OUT_FREQ);

  // Set COMP1 to control duty cycle
  LETIMER_CompareSet(LETIMER0, 1,
       CMU_ClockFreqGet(cmuClock_LETIMER0) * DUTY_CYCLE / (OUT_FREQ * 100));

  // Enable LETIMER0 output0 on PD6 (Route 0)
  LETIMER0->ROUTE |=  LETIMER_ROUTE_OUT0PEN | LETIMER_ROUTE_LOCATION_LOC0;

  // Initialize and enable LETIMER
  LETIMER_Init(LETIMER0, &letimerInit );
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
  initLETIMER();

  // Enter low energy state, PWM will continue
  // To change duty cycle, briefly wake from EM2 and change COMP1
  while (1)
  {
    EMU_EnterEM2(true);
  }
}
