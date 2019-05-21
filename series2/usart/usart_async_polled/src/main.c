/**************************************************************************//**
 * @main.c
 * @brief This project demonstrates use of the USART in polled mode.
 * The main loop waits until 80 characters or a carriage return are
 * received and then echos these back to the user.
 *
 * TIMER module. The GPIO pin specified in the readme.txt is configured for
 * output and after 3 seconds, CC0 sets the pin high.
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
  // Configure PA5 as an input (RX)
  GPIO_PinModeSet(gpioPortA, 5, gpioModeInput, 0);

  // Configure PA6 as an output (TX)
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    USART1 initialization
 *****************************************************************************/
void initUsart1(void)
{
  // Default asynchronous initializer (115.2 Kbps, 8N1, no flow control)
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

  // Route USART1 RX and TX to PA5 and PA6 pins, respectively
  GPIO->USARTROUTE[1].RXROUTE = (gpioPortA << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (5 << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[1].TXROUTE = (gpioPortA << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (6 << _GPIO_USART_TXROUTE_PIN_SHIFT);

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
