/**************************************************************************//**
 * @file
 * @brief This project demonstrates polled input capture using the WTIMER module.
 * The interrupt flag set by push button PB1 is polled, and the value of
 * the WTIMER0 is captured when PB1 is pressed.
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

#include <stdio.h>
#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_prs.h"
#include "bsp.h"

// Global variable to store input capture value
static int event_time;

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  //Enable GPIO and Peripheral Reflex System clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);

  // Configure PB0 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);

  // Select PB0 as interrupt source (Configure as disabled since using PRS)
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, false, false, false);

  // Select GPIO as source and PB0 as signal for PRS channel 0
  // Note that the PRS Channel Source Select splits LOWER (0-7) and HIGHER (8-15) GPIO pins
  if (BSP_GPIO_PB0_PIN < 8)
  {
    PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOL, BSP_GPIO_PB0_PIN, prsEdgeOff);
  }
  else
  {
    PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, BSP_GPIO_PB0_PIN, prsEdgeOff);
  }

  // Configure LED0 and LED1 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief WTIMER initialization
 *****************************************************************************/
void initWTIMER(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  // Configure WTIMER0 Compare/Capture for input capture of PRS channel 0
  TIMER_InitCC_TypeDef wtimerCCInit = TIMER_INITCC_DEFAULT;
  wtimerCCInit.mode = timerCCModeCapture;
  wtimerCCInit.prsInput = true;
  TIMER_InitCC(WTIMER0, 0, &wtimerCCInit);

  // Initialize wtimer with largest prescale setting
  TIMER_Init_TypeDef wtimerInit = TIMER_INIT_DEFAULT;
  wtimerInit.prescale = timerPrescale1024;
  TIMER_Init(WTIMER0, &wtimerInit);

  // Enable WTIMER0 interrupts
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);
  TIMER_IntEnable(WTIMER0, TIMER_IF_CC0);

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

  // Wait for interrupt flag from CC0
  while (!(TIMER_IntGetEnabled(WTIMER0) & TIMER_IF_CC0));

  // Clear CC0 interrupt flag
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);
  // Record input capture value
  event_time = TIMER_CaptureGet(WTIMER0, 0);
  // Indicate input capture occurred
  GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

  while (1)
  {
    EMU_EnterEM1();
  }
}
