/**************************************************************************//**
 * @file
 * @brief This project demonstrates the built-in PRS logic functions between channels.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2019 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
 ******************************************************************************/
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_prs.h"

#include "bsp.h"

#define PRS_CH   6

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Set Push Buttons as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);
  
  // Configure Push Buttons to create interrupt signals
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, 0, 0, false);
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN, 0, 0, false);

  // Set PC01 as output
  GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief PRS initialization
 *****************************************************************************/
void initPrs(void)
{
  // Enable PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Use Push Buttons as PRS source
  // Push Button 0
  PRS_SourceAsyncSignalSet(
        PRS_CH,
		PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
        BSP_GPIO_PB0_PIN);

  // Push Button 1
  PRS_SourceAsyncSignalSet(
        PRS_CH+1,
		PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
        BSP_GPIO_PB1_PIN);

  // Configure PRS logic
  PRS_Combine(PRS_CH+1, PRS_CH, prsLogic_A_OR_B);

  // Route output to PC01
  PRS_PinOutput(PRS_CH+1,prsTypeAsync, gpioPortC , 1);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Setup GPIO
  initGpio();
  
  // Setup PRS to route to output PC01
  initPrs();

  // Infinite loop
  while(1);
}
