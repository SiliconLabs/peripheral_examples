/**************************************************************************//**
 * @main_Series0.c
 * @brief This project reads input on the UART RX port and echoes it back to
 * the TX port one line at a time.  It does not use the VCOM or stdio.  See
 * readme.txt for details.
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
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_chip.h"

#define BUFFER_SIZE 80
char buffer[BUFFER_SIZE];

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
  char welcome_string[] = "Silicon Labs UART Code example!\r\f";
  int i,j;

  // Chip errata
  CHIP_Init();

  // Enable oscillator to GPIO and USART1 modules
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART1, true);

  // set pin modes for UART TX and RX pins
  GPIO_PinModeSet(gpioPortC, 1, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 1);

  // Initialize USART asynchronous mode and route pins
  USART_InitAsync(USART1, &init);
  USART1->ROUTE |= USART_ROUTE_TXPEN | USART_ROUTE_RXPEN;

  // print welcome message
  for (i = 0 ; welcome_string[i] != 0; i++)
  {
    USART_Tx(USART1, welcome_string[i]);
  }

  while (1)
  {
    // Read a line from the UART
    for (i = 0; i < BUFFER_SIZE - 1 ; i++ )
    {
      buffer[i] = USART_Rx(USART1);
      if (buffer[i] == '\r')
      {
        break; // Breaking on CR prevents it from being counted in the number of bytes
      }
    }

    // Echo the line back, adding CRLF
    for (j = 0; j < i ; j ++ )
    {
      USART_Tx(USART1, buffer[j]);
    }
    USART_Tx(USART1, '\r');
    USART_Tx(USART1, '\f');
  }
}
