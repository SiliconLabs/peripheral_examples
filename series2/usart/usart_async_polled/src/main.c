/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates use of the USART in polled mode. The main
 * loop waits until 80 characters or a carriage return are received and then
 * echos these back to the user.
 *
 * TIMER module. The GPIO pin specified in the readme.txt is configured for
 * output and after 3 seconds, CC0 sets the pin high.
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

// Size of the buffer for received data
#define BUFLEN  80

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{

  // Configure PA5 as an output (TX)
  GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 0);

  // Configure PA6 as an input (RX)
  GPIO_PinModeSet(gpioPortA, 6, gpioModeInput, 0);

  // Configure PB4 as output and set high (VCOM_ENABLE)
  // comment out next line to disable VCOM
  GPIO_PinModeSet(gpioPortB, 4, gpioModePushPull, 1);

}

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCmu(void)
{
  // Enable clock to GPIO and USART1
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART1, true);
}

/**************************************************************************//**
 * @brief
 *    USART1 initialization
 *****************************************************************************/
void initUsart1(void)
{
  // Default asynchronous initializer (115.2 Kbps, 8N1, no flow control)
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

  // Route USART1 TX and RX to PA5 and PA6 pins, respectively
  GPIO->USARTROUTE[1].TXROUTE = (gpioPortA << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (5 << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[1].RXROUTE = (gpioPortA << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (6 << _GPIO_USART_RXROUTE_PIN_SHIFT);

  // Enable RX and TX
  GPIO->USARTROUTE[1].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN;

  // Configure and enable USART1
  USART_InitAsync(USART1, &init);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  uint8_t buffer[BUFLEN];
  uint32_t i, j;

  // Chip errata
  CHIP_Init();

  // Initialize GPIO and USART1
  initCmu();
  initGpio();
  initUsart1();

  while (1)
  {
    // Zero out buffer
    for (i = BUFLEN; i > 0; --i)
      buffer[i] = 0;

    // Receive BUFLEN characters unless a new line is received first
    do
    {
      // Wait for a character
      buffer[i] = USART_Rx(USART1);

      // Exit loop on new line
      if (buffer[i] != '\r')
        i++;
      else
        break;
    }
    while (i < BUFLEN);

    // Output received characters
    for (j = 0; j < i; j++)
      USART_Tx(USART1, buffer[j]);
  }
}
