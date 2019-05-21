/**************************************************************************//**
 * @main_series1.c
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

// Note: change this to change the number of seconds to delay the pulse going high
#define NUM_SEC_DELAY 3.5

// Note: change this to change the pulse width (in units of s)
#define PULSE_WIDTH 1

// Compare values set to generate a 1 ms pulse with default clock and prescale
static uint32_t compareValue1;
static uint32_t compareValue2;

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure PA6 as output
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  uint32_t timerFreq = 0;
  // Initialize and start timer
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  // Configure TIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  timerInit.enable = false;
  timerInit.prescale = timerPrescale1024;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle;

  TIMER_Init(TIMER0, &timerInit);

  // Route Timer0 CC0 output to PA6
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortA << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (6 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);

  // Set the first compare value
  compareValue1 = (uint32_t)(timerFreq * NUM_SEC_DELAY);

  // Set first CCV value to trigger rising edge
  TIMER_CompareSet(TIMER0, 0, compareValue1);

  // Set the second compare value (don't actually use it, just set the global so
  // that it can be used by the handler later)
  compareValue2 = (timerFreq * PULSE_WIDTH);

  // Set buffered value to next be loaded into the CCV after overflow event
  TIMER_CompareBufSet(TIMER0, 0, compareValue2);

  // Safely enable TIMER0 CC0 interrupt flag
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);
  NVIC_DisableIRQ(TIMER0_IRQn);
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);

  // Enable the TIMER
  TIMER_Enable(TIMER0, true);
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
  while(!(TIMER_IntGet(TIMER0) & TIMER_IF_CC0));
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);

  // Disable timer after falling edge
  while(!(TIMER_IntGet(TIMER0) & TIMER_IF_CC0));
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);
  TIMER_Enable(TIMER0, false);

  while(1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}
