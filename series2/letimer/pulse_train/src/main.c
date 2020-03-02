/**************************************************************************//**
 * @main.c
 * @brief This project demonstrates generating a pulse train using the LETIMER
 * module. Expansion Header Pin 14 is configured for output compare
 * and toggles EH Pin 14 on each overflow event at a set frequency.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2020 Silicon Labs Inc. http://www.silabs.com</b>
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
#define OUT_FREQ 1000

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure PA6 (Expansion Header 14) as output
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCmu(void)
{
  // Enable clock to GPIO and LETIMER0
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_LETIMER0, true);
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

  // Reload top on underflow, pulse output, and run in free mode
  letimerInit.comp0Top = true;
  letimerInit.topValue = topValue;
  letimerInit.ufoa0 = letimerUFOAPulse;
  letimerInit.repMode = letimerRepeatFree;

  // Enable LETIMER0 output0 on PA6
  GPIO->LETIMERROUTE[0].ROUTEEN = GPIO_LETIMER_ROUTEEN_OUT0PEN;
  GPIO->LETIMERROUTE[0].OUT0ROUTE = \
      (gpioPortA << _GPIO_LETIMER_OUT0ROUTE_PORT_SHIFT) \
      | (6 << _GPIO_LETIMER_OUT0ROUTE_PIN_SHIFT);

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
  initCmu();
  initClock();
  initGpio();
  initLetimer();

  // Enter low energy state, pulse train will continue
  while (1)
  {
    EMU_EnterEM2(true);
  }
}
