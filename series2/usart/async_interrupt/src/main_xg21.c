/***************************************************************************//**
 * @file main_xg21.c
 *
 * @brief This project demonstrates asynchronous mode use of the USART
 * with interrupts.
 *
 * After initialization, the MCU goes into EM1 where the receive interrupt
 * handler buffers incoming data until the reception of 80 characters or a
 * CR (carriage return, ASCII 0x0D).
 *
 * Once the CR or 80 characters is hit, the receive data valid interrupt is
 * disabled, and the transmit buffer level interrupt, which, by default, is set
 * after power-on, is is enabled.  Each entry into the transmit interrupt
 * handler causes a character to be sent until all the characters originally
 * received have been echoed.
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
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

// BSP for board controller pin macros
#include "bsp.h"

// Size of the buffer for received data
#define BUFLEN  80

// Receive data buffer
uint8_t buffer[BUFLEN];

// Current position ins buffer
uint32_t inpos = 0;
uint32_t outpos = 0;

// True while receiving data (waiting for CR or BUFLEN characters)
bool receive = true;

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure the USART TX pin to the board controller as an output
  GPIO_PinModeSet(BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 0);

  // Configure the USART RX pin to the board controller as an input
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
 *    USART0 initialization
 *****************************************************************************/
void initUSART0(void)
{
  // Default asynchronous initializer (115.2 Kbps, 8N1, no flow control)
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

  // Route USART0 TX and RX to the board controller TX and RX pins
  GPIO->USARTROUTE[0].TXROUTE = (BSP_BCC_TXPORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (BSP_BCC_TXPIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].RXROUTE = (BSP_BCC_RXPORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (BSP_BCC_RXPIN << _GPIO_USART_RXROUTE_PIN_SHIFT);

  // Enable RX and TX signals now that they have been routed
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN;

  // Configure and enable USART0
  USART_InitAsync(USART0, &init);

  // Enable NVIC USART sources
  NVIC_ClearPendingIRQ(USART0_RX_IRQn);
  NVIC_EnableIRQ(USART0_RX_IRQn);
  NVIC_ClearPendingIRQ(USART0_TX_IRQn);
  NVIC_EnableIRQ(USART0_TX_IRQn);
}

/**************************************************************************//**
 * @brief
 *    The USART0 receive interrupt saves incoming characters.
 *****************************************************************************/
void USART0_RX_IRQHandler(void)
{
  // Get the character just received
  buffer[inpos] = USART0->RXDATA;

  // Exit loop on new line or buffer full
  if ((buffer[inpos] != '\r') && (inpos < BUFLEN))
    inpos++;
  else
    receive = false;   // Stop receiving on CR
}

/**************************************************************************//**
 * @brief
 *    The USART0 transmit interrupt outputs characters.
 *****************************************************************************/
void USART0_TX_IRQHandler(void)
{
  // Send a previously received character
  if (outpos < inpos)
    USART0->TXDATA = buffer[outpos++];
  else
  /*
   * Need to disable the transmit buffer level interrupt in this IRQ
   * handler when done or it will immediately trigger again upon exit
   * even though there is no data left to send.
   */
  {
    receive = true;   // Go back into receive when all is sent
    USART_IntDisable(USART0, USART_IEN_TXBL);
  }
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  uint32_t i;

  // Chip errata
  CHIP_Init();

  // Initialize GPIO and USART0
  initGPIO();
  initUSART0();

  while (1)
  {
    // Zero out buffer
    for (i = 0; i < BUFLEN; i++)
      buffer[i] = 0;

    // Enable receive data valid interrupt
    USART_IntEnable(USART0, USART_IEN_RXDATAV);

    // Wait in EM1 while receiving to reduce current draw
    while (receive)
      EMU_EnterEM1();

    // Disable receive data valid interrupt
    USART_IntDisable(USART0, USART_IEN_RXDATAV);

    // Enable transmit buffer level interrupt
    USART_IntEnable(USART0, USART_IEN_TXBL);

    // Wait in EM1 while transmitting to reduce current draw
    while (!receive)
      EMU_EnterEM1();

    // Reset buffer indices
    inpos = outpos = 0;
  }
}
