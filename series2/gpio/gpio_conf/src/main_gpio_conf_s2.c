/**************************************************************************//**
 * @file
 * @brief Demonstrates setting up simple input and output on GPIO
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2020 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "bsp.h"

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure Push Button 0 as input*/
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);

  /* Configure LED0 as a push pull for LED drive */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);

  while (1)
  {
    /* Check if button is pressed - when pressed, value will be 0 */
    if (!GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN))
    {
      GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    }
    else
    {
      GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    }
  }
}
