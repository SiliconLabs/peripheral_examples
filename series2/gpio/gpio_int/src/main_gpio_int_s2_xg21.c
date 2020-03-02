/**************************************************************************//**
 * @file
 * @brief Demonstrates basic interrupt functionality on GPIO using push buttons
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
#include "em_gpio.h"
#include "em_emu.h"
#include "bsp.h"
#include "retargetserial.h"
#include <stdio.h>

// must re-map GPIO to pins on port A/B for interrupts to work in EM2/3
#define  BUTTON_0_PORT  gpioPortA
#define  BUTTON_0_PIN   6

#define  BUTTON_1_PORT  gpioPortA
#define  BUTTON_1_PIN   5

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void gpioSetup(void)
{
  /* Configure Button PB0 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_PinModeSet(BUTTON_0_PORT, BUTTON_0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BUTTON_0_PORT, BUTTON_0_PIN, false, true, true);

  /* Configure Button PB1 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_PinModeSet(BUTTON_1_PORT, BUTTON_1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig (BUTTON_1_PORT, BUTTON_1_PIN, false, true, true);

  /* Enable EVEN interrupt to catch button press that changes slew rate */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  /* Enable ODD interrupt to catch button press that changes slew rate */
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  /* Configure pB00 as a push pull output for LED drive */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
  
  /* Configure pB01 as a push pull output for LED drive */
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even pins.
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Check if button 0 was pressed */
  if (interruptMask & (1 << BUTTON_0_PIN))
  {
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
  }
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even pins.
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Check if button 1 was pressed */
  if (interruptMask & (1 << BUTTON_1_PIN))
  {
    GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Initialize Push Buttons and PA0 */
  gpioSetup();

  /* Infinite loop */
  while (1)
  {
    /* Enter EM3 until one of the push-button interrupts triggers a wake-up */
    EMU_EnterEM3(false);
  }
}
