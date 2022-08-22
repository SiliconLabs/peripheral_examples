/***************************************************************************//**
 * @file main_xg27.c
 *
 * @brief This project demonstrates automatic baud detection with
 * interrupt-driven operation of the EUSART in asynchronous mode.
 *
 * After initialization, the MCU goes into EM1 and waits to receive the
 * sync byte 0x55 (ASCII character 'U') to set the baud rate.  Once the
 * baud rate is set, firmware disables the auto baud complete interrupt
 * and enables the RX FIFO level (RXFL) interrupt.  The main loop waits
 * until 80 characters or a carriage return (ASCII character 0x0D) are
 * received and then echos these back to the user.
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
#include "em_emu.h"
#include "em_eusart.h"
#include "em_gpio.h"

// BSP for board controller pin macros
#include "bsp.h"

// Size of the buffer for received data
#define BUFLEN  80

// Receive data buffer
uint8_t buffer[BUFLEN];

// Current position in buffer
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
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure the EUSART TX pin to the board controller as an output
  GPIO_PinModeSet(BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 1);

  // Configure the EUSART RX pin to the board controller as an input
  GPIO_PinModeSet(BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeInput, 0);

  // Route EUSART0 TX and RX to the board controller TX and RX pins
  GPIO->EUSARTROUTE[0].TXROUTE = (BSP_BCC_TXPORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (BSP_BCC_TXPIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].RXROUTE = (BSP_BCC_RXPORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
      | (BSP_BCC_RXPIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);

  // Enable RX and TX signals now that they have been routed
  GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN | GPIO_EUSART_ROUTEEN_TXPEN;

  /*
   * Configure the BCC_ENABLE pin as output and set high. This enables
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
  CMU_ClockEnable(cmuClock_EUSART0, true);

  // Default asynchronous initializer (8N1, no flow control)
  EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_HF;

  // Setting baudrate = 0 enables autobaud detection
  init.baudrate = 0;

  // Configure and enable EUSART0 for high-frequency (EM0/1) operation
  EUSART_UartInitHf(EUSART0, &init);

  // Enable NVIC EUSART sources
  NVIC_ClearPendingIRQ(EUSART0_RX_IRQn);
  NVIC_EnableIRQ(EUSART0_RX_IRQn);
  NVIC_ClearPendingIRQ(EUSART0_TX_IRQn);
  NVIC_EnableIRQ(EUSART0_TX_IRQn);
}

/**************************************************************************//**
 * @brief
 *    The EUSART0 receive interrupt saves incoming characters.
 *****************************************************************************/
void EUSART0_RX_IRQHandler(void)
{
  // Get the character just received
  buffer[inpos] = EUSART0->RXDATA;

  // Check for autobaud done interrupt
  if(EUSART_IntGet(EUSART0) & EUSART_IF_AUTOBAUDDONE) {
    // Disable Auto Baud Complete interrupt
    EUSART_IntDisable(EUSART0, EUSART_IEN_AUTOBAUDDONE);

    // Enable receive FIFO level interrupt
    EUSART_IntEnable(EUSART0, EUSART_IEN_RXFL);
  }
  // Otherwise buffer characters on RX FIFO interrupt
  else {
    // Exit loop on new line or buffer full
    if ((buffer[inpos] != '\r') && (inpos < BUFLEN)) {
      inpos++;
    }
    else {
      receive = false;   // Stop receiving on CR
    }
  }

  /*
   * The EUSART differs from the USART in that explicit clearing of
   * RX interrupt flags is required even after emptying the RX FIFO.
   */
  EUSART_IntClear(EUSART0, EUSART_IF_RXFL | EUSART_IF_AUTOBAUDDONE);
}

/**************************************************************************//**
 * @brief
 *    The EUSART0 transmit interrupt outputs characters.
 *****************************************************************************/
void EUSART0_TX_IRQHandler(void)
{
  // Send a previously received character
  if (outpos < inpos) {
    EUSART0->TXDATA = buffer[outpos++];

    /*
     * The EUSART differs from the USART in that the TX FIFO interrupt
     * flag must be explicitly cleared even after a write to the FIFO.
     */
    EUSART_IntClear(EUSART0, EUSART_IF_TXFL);
  }
  else {
  /*
   * Need to disable the transmit FIFO level interrupt in this IRQ
   * handler when done or it will immediately trigger again upon exit
   * even though there is no data left to send.
   */
    receive = true;   // Go back into receive when all is sent

    // Disable transmit FIFO level interrupt
    EUSART_IntDisable(EUSART0, EUSART_IEN_TXFL);
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

  // Initialize GPIO and EUSART0
  initGPIO();
  initEUSART0();

  // Enable interrupt on auto baud completion
  EUSART_IntEnable(EUSART0, EUSART_IEN_AUTOBAUDDONE);

  while (1) {
    // Zero out buffer
    for (i = 0; i < BUFLEN; i++) {
      buffer[i] = 0;
    }

    // Wait in EM1 while receiving to reduce current draw
    while (receive) {
      EMU_EnterEM1();
    }

    // Disable receive FIFO level interrupt
    EUSART_IntDisable(EUSART0, EUSART_IEN_RXFL);

    // Enable transmit FIFO level interrupt (defaults to one frame)
    EUSART_IntEnable(EUSART0, EUSART_IEN_TXFL);

    // Wait in EM1 while transmitting to reduce current draw
    while (!receive) {
      EMU_EnterEM1();
    }

    // Enable receive FIFO level interrupt
    EUSART_IntEnable(EUSART0, EUSART_IEN_RXFL);

    // Reset buffer indices
    inpos = outpos = 0;
  }
}
