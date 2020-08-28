/***************************************************************************//**
 * @file main_xgx2.c
 * @brief This project demonstrates use of the EUART with interrupts.
 *
 * After initialization, the MCU goes into EM1 where the receive interrupt
 * handler buffers incoming data until the reception of 80 characters or a
 * CR (carriage return, ASCII 0x0D).
 *
 * Once the CR or 80 characters is hit, the receive data valid interrupt is
 * disabled, and the transmit buffer level interrupt is is enabled.
 *
 * Each entry into the transmit interrupt handler causes a character to be sent 
 * until all the characters originally received have been echoed.
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

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_eusart.h"
#include "em_gpio.h"

// Size of the buffer for received data
#define BUFLEN  80

// Receive data buffer
uint8_t buffer[BUFLEN];

// Current position in buffer
uint32_t inpos = 0;
uint32_t outpos = 0;

// True while receiving data (waiting for CR or BUFLEN characters)
bool receive = true;

#define RETARGET_TXPORT      gpioPortC                    /* EUSART transmission port */
#define RETARGET_TXPIN       0                            /* EUSART transmission pin */
#define RETARGET_RXPORT      gpioPortC                    /* EUSART reception port */
#define RETARGET_RXPIN       1                            /* EUSART reception pin */
#define BAUDRATE             115200                       /* EUSART baudrate */

/**************************************************************************//**
 * @brief
 *    The EUART0 receive interrupt saves incoming characters.
 *****************************************************************************/
void EUART0_RX_IRQHandler(void)
{
  // Get the character just received
  buffer[inpos] = EUART0->RXDATA;

  // Exit loop on new line or buffer full
  if ((buffer[inpos] != '\r') && (inpos < BUFLEN))
    inpos++;
  else
    receive = false;   // Stop receiving on CR

  // Clear the requesting interrupt before exiting the handler
  EUSART_IntClear(EUART0, EUSART_IF_RXFLIF);
}

/**************************************************************************//**
 * @brief
 *    The EUART0 transmit interrupt outputs characters.
 *****************************************************************************/
void EUART0_TX_IRQHandler(void)
{
  // Send a previously received character
  if (outpos < inpos)
    EUART0->TXDATA = buffer[outpos++];
  else
  /*
   * Need to disable the transmit buffer level interrupt in this IRQ
   * handler when done or it will immediately trigger again upon exit
   * even though there is no data left to send.
   */
  {
    receive = true;   // Go back into receive when all is sent
    EUSART_IntDisable(EUART0, EUSART_IEN_TXFLIEN); // Disable TX FIFO Level Interrupt
  }
  
  // Clear the requesting interrupt before exiting the handler
  EUSART_IntClear(EUART0, EUSART_IF_TXFLIF);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO Clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure GPIO pins
  // To avoid false start, configure output as high
  GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1);
  GPIO_PinModeSet(RETARGET_RXPORT, RETARGET_RXPIN, gpioModeInput, 0);

  GPIO->EUARTROUTE->ROUTEEN = GPIO_EUART_ROUTEEN_TXPEN;
  GPIO->EUARTROUTE->TXROUTE = (RETARGET_TXPORT
                               << _GPIO_EUART_TXROUTE_PORT_SHIFT)
                              | (RETARGET_TXPIN << _GPIO_EUART_TXROUTE_PIN_SHIFT);
  GPIO->EUARTROUTE->RXROUTE = (RETARGET_RXPORT
                               << _GPIO_EUART_RXROUTE_PORT_SHIFT)
                              | (RETARGET_RXPIN << _GPIO_EUART_RXROUTE_PIN_SHIFT);
}

/**************************************************************************//**
 * @brief
 *    EUART initialization
 *****************************************************************************/
void initEUSART0(void)
{
  // Enable EUART0 Clock
  CMU_ClockSelectSet(cmuClock_EUART0, cmuSelect_EM01GRPACLK);
  CMU_ClockEnable(cmuClock_EM01GRPACLK, true);
  CMU_ClockEnable(cmuClock_EUART0, true);

  // Initialize the EUART0 module
  EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_HF;
  init.baudrate = BAUDRATE;
  EUSART_UartInitHf(EUART0, &init);

  // Enable NVIC USART sources
  NVIC_ClearPendingIRQ(EUART0_RX_IRQn);
  NVIC_EnableIRQ(EUART0_RX_IRQn);
  NVIC_ClearPendingIRQ(EUART0_TX_IRQn);
  NVIC_EnableIRQ(EUART0_TX_IRQn);
}

/**************************************************************************//**
 * @brief	Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialize GPIO and EUSART
  initGPIO();
  initEUSART0();

  while (1)
  {
    // Zero out buffer
    for (volatile uint32_t i = 0; i < BUFLEN; i++)
      buffer[i] = 0;

    // Enable receive data valid interrupt
    EUSART_IntEnable(EUART0, EUSART_IEN_RXFLIEN);

    // Wait in EM1 while receiving to reduce current draw
    while (receive)
      EMU_EnterEM1();

    // Disable RX FIFO Level Interrupt
    EUSART_IntDisable(EUART0, EUSART_IEN_RXFLIEN);
	
    // Enable TX FIFO Level Interrupt
    EUSART_IntEnable(EUART0, EUSART_IEN_TXFLIEN);

    // Wait in EM1 while transmitting to reduce current draw
    while (!receive)
      EMU_EnterEM1();

    // Reset buffer indices
    inpos = outpos = 0;
  }
}
