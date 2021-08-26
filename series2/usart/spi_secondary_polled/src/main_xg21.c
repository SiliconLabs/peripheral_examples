/***************************************************************************//**
 * @file main_xg21.c
 *
 * @brief This project demonstrates polled use of the USART in synchronous (SPI)
 * secondary mode. The main loop transmits the specified number of bytes and
 * receives the byte that is shifted in by the main with each outgoing one.
 * The incoming data overwrites the data buffer such that the initial transfer
 * of data is echoed during the next transfer.
 *
 * The pins used in this example are defined below and are described in the
 * accompanying readme.txt file.
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
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

// Size of the data buffers
#define BUFLEN  10

// SPI ports and pins
#define US0MISO_PORT  gpioPortC
#define US0MISO_PIN   1
#define US0MOSI_PORT  gpioPortC
#define US0MOSI_PIN   0
#define US0CLK_PORT   gpioPortC
#define US0CLK_PIN    2
#define US0CS_PORT    gpioPortC
#define US0CS_PIN     3

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
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
void initUSART0(void)
{
  // Default asynchronous initializer (main mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  init.master = false;  // Operate as a secondary
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

  // Run from the HFXO to maximize performance
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  CMU_HFXOInit(&hfxoInit);

  // Set DPLL to 76.8 MHz (38.4 MHz PCLK)
  CMU_DPLLInit_TypeDef initDPLL = CMU_DPLL_HFXO_TO_76_8MHZ;

  // Attempt DPLL lock; halt on failure
  if (CMU_DPLLLock(&initDPLL) == false)
    __BKPT(0);

  /*
   * Set the optimal number of wait state for the selected HFCLK
   * frequency.  Note that the second parameter, which specifies the
   * voltage scaling level, is meaningless on Series 0 devices.
   */
  CMU_UpdateWaitStates(SystemCoreClockGet(), 0);

  // Zero buffer
  for (i = 0; i < BUFLEN; i++)
    buffer[i] = 0;

  // Initialize GPIO and USART0
  initGPIO();
  initUSART0();

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
