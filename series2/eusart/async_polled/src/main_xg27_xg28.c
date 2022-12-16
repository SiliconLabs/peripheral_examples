/***************************************************************************//**
 * @file main_xg27_xg28.c
 *
 * @brief This project demonstrates use of the EUSART in polled mode. The main
 * loop waits until 80 characters or a carriage return are received and then
 * echos these back to the user.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stddef.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_eusart.h"
#include "em_gpio.h"

// BSP for board controller pin macros
#include "bsp.h"

// Size of the buffer for received data
#define BUFLEN  80

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCMU(void)
{
  // Enable clock to GPIO and EUSART0
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_EUSART0, true);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure the EUSART TX pin to the board controller as an output
  GPIO_PinModeSet(BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 1);

  // Configure the EUSART RX pin to the board controller as an input
  GPIO_PinModeSet(BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeInput, 0);

  /*
   * Configure the BCC_ENABLE pin as output and set high.  This enables
   * the virtual COM port (VCOM) connection to the board controller and
   * permits serial port traffic over the debug connection to the host
   * PC.
   *
   * To disable the VCOM connection and use the pins on the kit
   * expansion (EXP) header, comment out the following line.
   */
  GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief
 *    EUSART0 initialization
 *****************************************************************************/
void initEUSART0(void)
{
  // Default asynchronous initializer (115.2 Kbps, 8N1, no flow control)
  EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_HF;

  // Route EUSART0 TX and RX to the board controller TX and RX pins
  GPIO->EUSARTROUTE[0].TXROUTE = (BSP_BCC_TXPORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
            | (BSP_BCC_TXPIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].RXROUTE = (BSP_BCC_RXPORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
            | (BSP_BCC_RXPIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);

  // Enable RX and TX signals now that they have been routed
  GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN | GPIO_EUSART_ROUTEEN_TXPEN;

  // Configure and enable EUSART0 for high-frequency (EM0/1) operation
  EUSART_UartInitHf(EUSART0, &init);
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

  // Initialize GPIO and EUSART0
  initCMU();
  initGPIO();
  initEUSART0();

  while (1)
  {
    // Zero out buffer
    for (i = BUFLEN; i > 0; --i)
      buffer[i] = 0;

    // Receive BUFLEN characters unless a new line is received first
    do
    {
      // Wait for a character
      buffer[i] = EUSART_Rx(EUSART0);

      // Exit loop on new line
      if (buffer[i] != '\r')
        i++;
      else
        break;
    }
    while (i < BUFLEN);

    // Output received characters
    for (j = 0; j < i; j++)
      EUSART_Tx(EUSART0, buffer[j]);
  }
}
