/***************************************************************************//**
 * @file main_s1_xg11.c
 * @brief This project reads input on the UART RX port and echoes it back to the
 * TX port one line at a time. It does not use the VCOM or stdio. See readme.txt
 * for details.
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

  // Enable oscillator to GPIO and USART0 modules
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART0, true);

  // set pin modes for UART TX and RX pins
  GPIO_PinModeSet(gpioPortC, 1, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 1);

  // Initialize USART asynchronous mode and route pins
  USART_InitAsync(USART0, &init);
  USART0->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC5 | USART_ROUTELOC0_TXLOC_LOC5;
  USART0->ROUTEPEN |= USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

  // print welcome message
  for (i = 0 ; welcome_string[i] != 0; i++)
  {
    USART_Tx(USART0, welcome_string[i]);
  }

  while (1)
  {
    // Read a line from the UART
    for (i = 0; i < BUFFER_SIZE - 1 ; i++ )
    {
      buffer[i] = USART_Rx(USART0);
      if (buffer[i] == '\r')
      {
        break; // Breaking on CR prevents it from being counted in the number of bytes
      }
    }

    // Echo the line back, adding CRLF
    for (j = 0; j < i ; j ++ )
    {
      USART_Tx(USART0, buffer[j]);
    }
    USART_Tx(USART0, '\r');
    USART_Tx(USART0, '\f');
  }
}
