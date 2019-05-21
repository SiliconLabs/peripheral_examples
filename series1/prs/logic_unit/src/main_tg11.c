/**************************************************************************//**
 * @file
 * @brief This project demonstrates the built-in PRS logic functions between channels.
 * PB0 and PB1 are routed to output pins.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_prs.h"

#include "bsp.h"

#define PRS_BASE_CH   0
#define USE_AND       true

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable clock for GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set Push Buttons to input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);

  // Set LEDs to output
  GPIO_PinModeSet(gpioPortC, 14, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 15, gpioModePushPull, 0);

  // Configure Push Buttons to create interrupt signals
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, 0, 0, false);
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN, 0, 0, false);
}

/**************************************************************************//**
 * @brief PRS initialization
 *****************************************************************************/
void initPrs(void)
{
  // Enable PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Use Push Buttons as PRS triggers in EM2
  // The PRS uses GPIOL for gpio pins 0-7 and GPIOH for gpio pins 8-15
  // Push Button 0
  if (BSP_GPIO_PB0_PIN > 7){
    PRS_SourceAsyncSignalSet(
        PRS_BASE_CH,
        PRS_CH_CTRL_SOURCESEL_GPIOH,
        (uint32_t)(BSP_GPIO_PB0_PIN - 8));
  }
  else {
    PRS_SourceAsyncSignalSet(
        PRS_BASE_CH,
        PRS_CH_CTRL_SOURCESEL_GPIOL,
        BSP_GPIO_PB0_PIN);
  }

  // Push Button 1
  if (BSP_GPIO_PB1_PIN > 7){
    PRS_SourceAsyncSignalSet(
        PRS_BASE_CH+1,
        PRS_CH_CTRL_SOURCESEL_GPIOH,
        (uint32_t)(BSP_GPIO_PB1_PIN - 8));
  }
  else {
    PRS_SourceAsyncSignalSet(
        PRS_BASE_CH+1,
        PRS_CH_CTRL_SOURCESEL_GPIOL,
        BSP_GPIO_PB1_PIN);
  }

  // Configure PRS Logic
  if (USE_AND)
  {
    PRS->CH[PRS_BASE_CH].CTRL |= PRS_CH_CTRL_ANDNEXT; // Channel 0 will AND with Channel 1
  }
  else
  {
    PRS->CH[PRS_BASE_CH + 1].CTRL |= PRS_CH_CTRL_ORPREV; // Channel 1 will OR with Channel 0
  }

  // Route PRS output to location 2 (Expansion header pins)
  PRS_GpioOutputLocation(PRS_BASE_CH, 2);   // PC 14, EH 12
  PRS_GpioOutputLocation(PRS_BASE_CH+1, 2); // PC 15, EH 14
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Setup GPIO
  initGpio();
  // Setup PRS to route to LEDs
  initPrs();

  // Infinite loop
  while(1)
  {
    // Enter EM2
    EMU_EnterEM2(false);
  }
}
