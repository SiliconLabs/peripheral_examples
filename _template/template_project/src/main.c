/**************************************************************************//**
 * @file
 * @brief Brief, kit-agnostic description here. This project demonstrates the 
 * GPIOs (push buttons and LEDs) and the USART in asynchronous mode. USART is 
 * connected the board controller, which enumerates as a VCOM port when 
 * connected to a PC. See readme.txt for details.
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
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "retargetserial.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB0 and PB1 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 0);
  
  // Configure LED0 and LED1 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
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

  // Initialize USART, enable board controller VCOM function, map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  printf("Hello world\n");

  while (1)
  {
  }
}
