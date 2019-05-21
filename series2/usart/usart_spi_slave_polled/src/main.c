/**************************************************************************//**
 * @main.c
 * @brief This project demonstrates polled use of the USART in synchronous
 * (SPI) slave mode.  The main loop transmits the specified number of bytes
 * and receives the byte that is shifted in by the master with each outgoing
 * one.  The incoming data overwrites the data buffer such that the initial
 * transfer of data is echoed * during the next transfer.
 *
 * The pins used in this example are defined below and are described in
 * the accompanying readme.txt file.
 *
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

// Size of the data buffers
#define BUFLEN  10

// Ports and pins for SPI interface
#define US0MISO_PORT  gpioPortA
#define US0MISO_PIN   5
#define US0MOSI_PORT  gpioPortA
#define US0MOSI_PIN   6
#define US0CLK_PORT   gpioPortC
#define US0CLK_PIN    3
#define US0CS_PORT    gpioPortC
#define US0CS_PIN     1

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure MOSI pin as an input
  GPIO_PinModeSet(US0MOSI_PORT, US0MOSI_PIN, gpioModeInput, 0);

  // Configure MISO pin as an output
  GPIO_PinModeSet(US0MISO_PORT, US0MISO_PIN, gpioModePushPull, 0);

  // Configure CLK pin as an input
  GPIO_PinModeSet(US0CLK_PORT, US0CLK_PIN, gpioModeInput, 0);

  // Configure CS pin as an input pulled high
  GPIO_PinModeSet(US0CS_PORT, US0CS_PIN, gpioModeInput, 1);

  /*
   * Configure CS pin for high-to-low edge detection, but don't enable
   * the interrupt.
   */
  GPIO_ExtIntConfig(US0CS_PORT, US0CS_PIN, US0CS_PIN, false, true, false);
}

/**************************************************************************//**
 * @brief
 *    USART0 initialization
 *****************************************************************************/
void initUsart0(void)
{
  // Default asynchronous initializer (master mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  init.master = false;  // Operate as a slave
  init.msbf = true;     // MSB first transmission for SPI compatibility

  /*
   * Route USART0 RX, TX, and CLK to the specified pins.  Note that CS is
   * not controlled by USART0 so there is no write to the corresponding
   * USARTROUTE register to do this.
   */
  GPIO->USARTROUTE[0].RXROUTE = (US0MISO_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (US0MISO_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].TXROUTE = (US0MOSI_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (US0MOSI_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CLKROUTE = (US0CLK_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (US0CLK_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CSROUTE = (US0CS_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (US0CS_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN |
                                GPIO_USART_ROUTEEN_TXPEN |
                                GPIO_USART_ROUTEEN_CLKPEN |
                                GPIO_USART_ROUTEEN_CSPEN;

  // Configure and enable USART0
  USART_InitSync(USART0, &init);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  uint32_t i;

  // Data buffer
  uint8_t buffer[BUFLEN];

  // Chip errata
  CHIP_Init();

  // Zero buffer
  for (i = 0; i < BUFLEN; i++)
    buffer[i] = 0;

  // Initialize GPIO and USART0
  initGpio();
  initUsart0();

  while (1)
  {
    // Clear the flag and wait for a falling edge on US0CS_PIN
    GPIO_IntClear(1 << US0CS_PIN);

    while ((GPIO_IntGet() & (1 << US0CS_PIN)) == 0);

    /*
     * After getting the falling edge, clear the RX buffer before
     * receiving the desired number of bytes.
     */
    USART0->CMD = USART_CMD_CLEARRX;

    /*
     * Perform the specified number of single byte SPI transfers
     * (transmission and reception).
     *
     * Data to be queued for transmission is sent with USART_Tx(), which
     * checks that there is room in the transmit FIFO but does not
     * actually wait for transmit complete (USART_STATUS_TXC).
     *
     * Instead, incoming data is read from the FIFO with USART_Rx(),
     * which polls USART_STATUS,RXDATAV to make sure data has arrived.
     */
    for (i = 0; i < BUFLEN; i++)
    {
      USART_Tx(USART0, buffer[i]);
      buffer[i] = USART_Rx(USART0);
    }
  }
}
