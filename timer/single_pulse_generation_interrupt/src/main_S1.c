/**************************************************************************//**
 * @file
 * @brief This project demonstrates the generation of a single pulse using
 * output compare. The TIMER module is configured for output compare such that
 * a 10ms pulse is generated on PC10 (Expansion Header Pin 16) after a 3 second
 * delay.
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

// First compare edge for 3 second delay
// 55664 / (19MHz / 1024) = 3 seconds
#define COMPARE_VALUE1 55664
// Second compare edge for 1 ms pulse
// (20 - 1) / (19MHz / 1024) = 1 millisecond
#define COMPARE_VALUE2 (COMPARE_VALUE1 + 20)

void TIMER0_IRQHandler(void)
{
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);

  // Load Compare buffer with second edge value
  TIMER_CompareSet(TIMER0, 0, COMPARE_VALUE2);
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
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure TIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  // Toggle output on compare match
  timerCCInit.cmoa = timerOutputActionToggle;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set route to Location 15 and enable
  // TIM0_CC0 #15 is PC10 (Expansion Header Pin 16)
  TIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC15;
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Load first
  TIMER_CompareSet(TIMER0, 0, COMPARE_VALUE1);

  // Initialize and start timer with highest prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1024;
  // Generate only one pulse
  timerInit.oneShot = true;
  TIMER_Init(TIMER0, &timerInit);

  // Safely Enable TIMER0 interrupts
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);
  NVIC_DisableIRQ(TIMER0_IRQn);
  TIMER_IntEnable(TIMER0, TIMER_IF_CC0);
  NVIC_EnableIRQ(TIMER0_IRQn);
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
