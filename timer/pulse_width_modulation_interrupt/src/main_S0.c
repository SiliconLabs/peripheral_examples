/**************************************************************************//**
 * @file
 * @brief This project demonstrates pulse width modulation using the TIMER 
 * module. PD6 (See readme for Exp. pin out) is configured for output and outputs
 * a 1kHz, 30% duty cycle signal. The duty cycle can be adjusted by writing to
 * the CCVB or changing the global duty_cycle_percentage variable.
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
#include "bsp.h"

// Output frequency in Hz
#define PWM_FREQ 1000

// Desired percent duty cycle, used to update CCVB value
volatile int duty_cycle_percent = 30;

/**************************************************************************//**
 * @brief TIMER1_IRQHandler
 * Interrupt Service Routine TIMER1 Interrupt Line
 *****************************************************************************/
void TIMER1_IRQHandler(void)
{
  // Get pending flags and clear
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);

  // Update CCVB to alter duty cycle starting next period
  TIMER_CompareBufSet(TIMER1, 0, (TIMER_TopGet(TIMER1) * duty_cycle_percent) / 100);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD6 (See readme for Exp. pin out) as output
  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Configure TIMER1 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  // use PWM mode, which sets output on overflow and clears on compare events
  timerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Route TIMER1 CC0 to location 4 and enable CC0 route pin
  // TIM1_CC0 #4 is GPIO Pin PD6 (Expansion Header Pin 16)
  TIMER1->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC4);

  // Set top value to overflow once per signal period
  TIMER_TopSet(TIMER1, CMU_ClockFreqGet(cmuClock_TIMER1) / PWM_FREQ);

  // Set compare value for initial duty cycle
  TIMER_CompareSet(TIMER1, 0, (TIMER_TopGet(TIMER1) * duty_cycle_percent) / 100);

  // Initialize and start timer with no prescaling
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1;
  TIMER_Init(TIMER1, &timerInit);

  // Safely enable TIMER1 CC0 interrupt
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(TIMER1_IRQn);

  // Interrupt on compare event to set CCVB to update duty cycle on next period
  TIMER_IntEnable(TIMER1, TIMER_IF_CC0);
  NVIC_EnableIRQ(TIMER1_IRQn);
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

  while (1)
  {
    EMU_EnterEM1();
  }
}
