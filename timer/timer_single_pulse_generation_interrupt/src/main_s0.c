/**************************************************************************//**
 * @main_series0.c
 * @brief This project demonstrates the generation of a single pulse using
 * output compare. The TIMER module is configured for output compare such that
 * a 1ms pulse is generated on the GPIO pin specified in the readme.txt after
 * a 1 second delay.
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
#define NUM_SEC_DELAY 1

// Note: change this to change the timer prescale
#define TIMER_PRESCALE timerPrescale1024

// Note: change this to change the pulse width (in units of ms)
#define PULSE_WIDTH 1

// Compare values for outputting the rising and falling edge
static uint32_t compareValue1;
static uint32_t compareValue2;

/**************************************************************************//**
 * @brief
 *    TIMER1 handler
 *****************************************************************************/
void TIMER1_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(TIMER1);
  TIMER_IntClear(TIMER1, flags);

  // Load compare register with second compare value
  TIMER_CompareSet(TIMER1, 0, compareValue2);
}

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
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Configure TIMER1 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle; // Toggle output on compare match
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Set route to Location 4 and enable
  // TIM1_CC0 #4 is PD6
  TIMER1->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC4);;

  // Initialize and start timer with highest prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.enable = false;
  timerInit.prescale = TIMER_PRESCALE;
  timerInit.oneShot = true; // Generate only one pulse
  TIMER_Init(TIMER1, &timerInit);

  // Set the first compare value
  compareValue1 = CMU_ClockFreqGet(cmuClock_TIMER1)
                    * NUM_SEC_DELAY
                    / (1 << TIMER_PRESCALE);
  TIMER_CompareSet(TIMER1, 0, compareValue1);

  // Set the second compare value (don't actually use it, just set the global so
  // that it can be used by the handler later)
  compareValue2 = (CMU_ClockFreqGet(cmuClock_TIMER1)
                    * PULSE_WIDTH
                    / 1000
                    / (1 << TIMER_PRESCALE))
                    + compareValue1;

  // Enable TIMER1 interrupts
  TIMER_IntEnable(TIMER1, TIMER_IEN_CC0);
  NVIC_EnableIRQ(TIMER1_IRQn);

  // Enable the TIMER
  TIMER_Enable(TIMER1, true);
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

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

