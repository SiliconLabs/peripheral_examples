/***************************************************************************//**
 * @file main_chA_chB.c
 * @brief This project demonstrates the built-in PRS logic functions between
 * channels.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_emu.h"

#include "bsp.h"

#define PRS_CH_A   6
#define PRS_CH_B   1

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set Push Buttons as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter,
                  1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter,
                  1);
  
  // Configure Push Buttons to create interrupt signals
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, 0,
                    0, false);
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN, 0,
                    0, false);

  // Set LED1 as output
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
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
        PRS_CH_B,
        PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
        BSP_GPIO_PB0_PIN);

  // Push Button 1
  PRS_SourceAsyncSignalSet(
        PRS_CH_A,
        PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
        BSP_GPIO_PB1_PIN);

  // Configure PRS logic
  PRS_Combine(PRS_CH_A, PRS_CH_B, prsLogic_A_OR_B);

  // Route output to LED1
  PRS_PinOutput(PRS_CH_A, prsTypeAsync, BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
  /* Note that there are certain restrictions to where a PRS channel can be
   *   routed. Consult the datasheet of the device to see if a channel can be
   *   routed to the requested GPIO pin.
   */
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Setup GPIO
  initGpio();
  
  // Setup PRS to route to output on LED1
  initPrs();

  // Infinite loop
  while(1)
  {
    // Stays in EM1 for max power saving
    EMU_EnterEM1();
  }
}
