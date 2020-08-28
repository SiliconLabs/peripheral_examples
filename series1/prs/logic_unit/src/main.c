/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the built-in PRS logic functions between
 * channels. PB0 and PB1 are routed to output pins.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
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
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

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

  // Route PRS output to location 4 (LEDs)
  PRS_GpioOutputLocation(PRS_BASE_CH, 4);
  PRS_GpioOutputLocation(PRS_BASE_CH+1, 4);
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
