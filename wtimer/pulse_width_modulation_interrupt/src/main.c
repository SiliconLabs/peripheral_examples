/**************************************************************************//**
 * @file
 * @brief This project demonstrates pulse width modulation using the WTIMER 
 * module. PC10 (Expansion Header Pin 16) is configured for output and outputs
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
 * @brief WTIMER0_IRQHandler
 * Interrupt Service Routine WTIMER0 Interrupt Line
 *****************************************************************************/
void WTIMER0_IRQHandler(void)
{
  // Get pending flags and clear
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);

  // Update CCVB to alter duty cycle starting next period
  TIMER_CompareBufSet(WTIMER0, 0, (TIMER_TopGet(WTIMER0) * duty_cycle_percent) / 100);
}

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
  // use PWM mode, which sets output on overflow and clears on compare events
  wtimerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(WTIMER0, 0, &wtimerCCInit);

  // Set route to Location 15 and enable
  // WTIM0_CC0 #30 is PC10 (Expansion Header Pin 16)
  WTIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC30;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Set top value to overflow once per signal period
  TIMER_TopSet(WTIMER0, CMU_ClockFreqGet(cmuClock_WTIMER0) / PWM_FREQ);

  // Set compare value for initial duty cycle
  TIMER_CompareSet(WTIMER0, 0, (TIMER_TopGet(WTIMER0) * duty_cycle_percent) / 100);

  // Initialize and start wtimer with no prescaling
  TIMER_Init_TypeDef wtimerInit = TIMER_INIT_DEFAULT;
  wtimerInit.prescale = timerPrescale1;
  TIMER_Init(WTIMER0, &wtimerInit);

  // Safely enable WTIMER0 CC0 interrupt
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(WTIMER0_IRQn);

  // Interrupt on compare event to set CCVB to update duty cycle on next period
  TIMER_IntEnable(WTIMER0, TIMER_IF_CC0);
  NVIC_EnableIRQ(WTIMER0_IRQn);
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
