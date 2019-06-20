/**************************************************************************//**
 * @main.c
 * @brief This project demonstrates output compare in EM2 using the LETIMER.
 * After 1 second an output pin is set high (see README)
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2019 Silicon Labs Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 *****************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "bsp.h"

// Desired frequency in Hz
#define OUT_FREQ 1000

// Desired repeat count
#define REPEAT_COUNT 10

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure PA6 (Expansion Header 14) as output
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief Clock initialization
 *****************************************************************************/
void initClock(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

  // Select LFXO for the LETIMER
  CMU_LFXOInit(&lfxoInit);
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
}

/**************************************************************************//**
 * @brief LETIMER initialization
 *****************************************************************************/
void initLetimer(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Calculate the top value (frequency) based on clock source
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / OUT_FREQ;

  // Reload top on underflow, set output, and run in one-shot mode
  letimerInit.comp0Top = true;
  letimerInit.topValue = topValue;
  letimerInit.ufoa0 = letimerUFOAToggle;
  letimerInit.repMode = letimerRepeatOneshot;

  // Enable LETIMER0 output0 on PA6
  GPIO->LETIMERROUTE[0].ROUTEEN = GPIO_LETIMER_ROUTEEN_OUT0PEN;
  GPIO->LETIMERROUTE[0].OUT0ROUTE = \
      (gpioPortA << _GPIO_LETIMER_OUT0ROUTE_PORT_SHIFT) \
      | (6 << _GPIO_LETIMER_OUT0ROUTE_PIN_SHIFT);

  // repeat 10
  LETIMER_RepeatSet(LETIMER0, 0, REPEAT_COUNT);

  // Initialize and enable LETIMER
  LETIMER_Init(LETIMER0, &letimerInit);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initClock();
  initGpio();
  initLetimer();

  // Enter low energy state
  while (1)
  {
    EMU_EnterEM2(true);
  }
}
