/**************************************************************************//**
 * @main.c
 * @brief This project demonstrates use of the USART with interrupts.
 *
 * After initialization, the MCU goes into EM1 where the receive interrupt
 * handler buffers incoming data until the reception of 80 characters or a
 * CR (carriage return, ASCII 0x0D).
 *
 * Once the CR or 80 characters is hit, the receive data valid interrupt is
 * disabled, and the transmit buffer level interrupt, which, by default, is
 * set after power-on, is is enabled.  Each entry into the transmit
 * interrupt handler causes a character to be sent until all the
 * characters originally received have been echoed.
 *
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2019 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_usart.h"

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

  // Enable RX and TX signals now that they have been routed
  GPIO->USARTROUTE[1].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN;

  // Configure and enable USART1
  USART_InitAsync(USART1, &init);

  // Enable NVIC USART sources
  NVIC_ClearPendingIRQ(USART1_RX_IRQn);
  NVIC_EnableIRQ(USART1_RX_IRQn);
  NVIC_ClearPendingIRQ(USART1_TX_IRQn);
  NVIC_EnableIRQ(USART1_TX_IRQn);
}

/**************************************************************************//**
 * @brief
 *    The USART1 receive interrupt saves incoming characters.
 *****************************************************************************/
void USART1_RX_IRQHandler(void)
{
  // Get the character just received
  buffer[inpos] = USART1->RXDATA;

  // Exit loop on new line or buffer full
  if ((buffer[inpos] != '\r') && (inpos < BUFLEN))
    inpos++;
  else
    receive = false;   // Stop receiving on CR

  // Clear the requesting interrupt before exiting the handler
  USART_IntClear(USART1, USART_IF_RXDATAV);
}

/**************************************************************************//**
 * @brief
 *    The USART1 transmit interrupt outputs characters.
 *****************************************************************************/
void USART1_TX_IRQHandler(void)
{
  // Send a previously received character
  if (outpos < inpos)
    USART1->TXDATA = buffer[outpos++];
  else
  /*
   * Need to disable the transmit buffer level interrupt in this IRQ
   * handler when done or it will immediately trigger again upon exit
   * even though there is no data left to send.
   */
  {
    receive = true;   // Go back into receive when all is sent
    USART_IntDisable(USART1, USART_IEN_TXBL);
  }
  // Clear the requesting interrupt before exiting the handler
  USART_IntClear(USART1, USART_IF_TXBL);
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

  // Initialize GPIO and USART1
  initGpio();
  initUsart1();

  while (1)
  {
    // Zero out buffer
    for (i = 0; i < BUFLEN; i++)
      buffer[i] = 0;

    // Enable receive data valid interrupt
    USART_IntEnable(USART1, USART_IEN_RXDATAV);

    // Wait in EM1 while receiving to reduce current draw
    while (receive)
      EMU_EnterEM1();

    // Disable receive data valid interrupt
    USART_IntDisable(USART1, USART_IEN_RXDATAV);

    // Enable transmit buffer level interrupt
    USART_IntEnable(USART1, USART_IEN_TXBL);

    // Wait in EM1 while transmitting to reduce current draw
    while (!receive)
      EMU_EnterEM1();

    // Reset buffer indices
    inpos = outpos = 0;
  }
}
