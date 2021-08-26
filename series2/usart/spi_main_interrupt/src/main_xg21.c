/***************************************************************************//**
 * @file main_xg21.c
 *
 * @brief This project demonstrates interrupt-driven use of the USART in
 * synchronous (SPI) main mode. The main loop transmits the specified number
 * of bytes and receives the byte that is shifted in with each outgoing one.
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
#include "em_emu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "bsp.h"

// SPI ports and pins
#define US0MOSI_PORT  gpioPortC
#define US0MOSI_PIN   0
#define US0MISO_PORT  gpioPortC
#define US0MISO_PIN   1
#define US0CLK_PORT   gpioPortC
#define US0CLK_PIN    2
#define US0CS_PORT    gpioPortC
#define US0CS_PIN     3

// Size of the data buffers
#define BUFLEN  10

// Outgoing data
uint8_t outbuf[BUFLEN];

// Incoming data
uint8_t inbuf[BUFLEN];

// Position in the buffer
uint32_t bufpos;

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure MOSI pin as an output
  GPIO_PinModeSet(US0MOSI_PORT, US0MOSI_PIN, gpioModePushPull, 0);

  // Configure MISO pin as an input
  GPIO_PinModeSet(US0MISO_PORT, US0MISO_PIN, gpioModeInput, 0);

  // Configure CLK pin as an output
  GPIO_PinModeSet(US0CLK_PORT, US0CLK_PIN, gpioModePushPull, 0);

  // Configure CS pin as an output initially high
  GPIO_PinModeSet(US0CS_PORT, US0CS_PIN, gpioModePushPull, 1);

  // Configure button 0 pin as an input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);

  // Interrupt on button 0 rising edge to start transfers
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, true, false, true);

  // Enable NVIC GPIO interrupt
#if (BSP_GPIO_PB0_PIN & 1)
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
#else
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
#endif
}

/**************************************************************************//**
 * @brief
 *    USART0 initialization
 *****************************************************************************/
void initUSART0(void)
{
  // Default asynchronous initializer (main mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  init.msbf = true;     // MSB first transmission for SPI compatibility

  // Configure and enable USART0
  USART_InitSync(USART0, &init);

  /*
   * Route USART0 RX, TX, and CLK to the specified pins.  Note that CS
   * is not controlled by the USART in this case but as a GPIO under
   * software control in the main loop.
   */
  GPIO->USARTROUTE[0].TXROUTE = (US0MOSI_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (US0MOSI_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].RXROUTE = (US0MISO_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (US0MISO_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CLKROUTE = (US0CLK_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (US0CLK_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN  |    // MISO
                                GPIO_USART_ROUTEEN_TXPEN  |    // MOSI
                                GPIO_USART_ROUTEEN_CLKPEN;

  // Enable NVIC USART sources
  NVIC_ClearPendingIRQ(USART0_TX_IRQn);
  NVIC_EnableIRQ(USART0_TX_IRQn);

  // Enable transmit complete interrupt
  USART_IntEnable(USART0, USART_IEN_TXC);
}

/**************************************************************************//**
 * @brief GPIO IRQHandler
 *****************************************************************************/
#if (BSP_GPIO_PB0_PIN & 1)
void GPIO_ODD_IRQHandler(void)
#else
void GPIO_EVEN_IRQHandler(void)
#endif
{
  // Disable button 0 interrupt since it is only needed once
  GPIO_IntDisable(1 << BSP_GPIO_PB0_PIN);

  // Clear the falling edge interrupt flag
  GPIO_IntClear(1 << BSP_GPIO_PB0_PIN);
}

/**************************************************************************//**
 * @brief
 *    USART0 transmit interrupt handler
 *****************************************************************************/
void USART0_TX_IRQHandler(void)
{
  /*
   * Save byte received concurrent with the transmission of the last bit of
   * the previous outgoing byte, and increment the buffer position to the
   * next byte.
   */
  inbuf[bufpos++] = USART0->RXDATA;

  // If there are still bytes left to send, transmit the next one
  if (bufpos < BUFLEN)
    USART0->TXDATA = outbuf[bufpos];

  // Clear the requesting interrupt before exiting the handler
  USART_IntClear(USART0, USART_IF_TXC);
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

  /*
   * Wait for button 0 press before starting.  This happens one time so
   * that the device running the matching secondary code can be started
   * first, otherwise it would be difficult to keep everything
   * synchronized without a separate handshaking mechanism.
   */
  EMU_EnterEM1();

  while (1)
  {
    // Zero incoming buffer and populate outgoing data array
    for (i = 0; i < BUFLEN; i++)
    {
      inbuf[i] = 0;
      outbuf[i] = (uint8_t)i;
    }

    // Start at the beginning of the buffer
    bufpos = 0;

    // Assert chip select (drive low)
    GPIO_PinOutClear(US0CS_PORT, US0CS_PIN);

    /*
     * Because this example is most likely going to be running with
     * another EFM32/EFR32 device on the secondary side, it must insert
     * a delay between chip select assertion and sending the first
     * byte.
     *
     * On Series 1 and Series 2 EFM32/EFR32 devices, this delay needs
     * to be between 7 and 10 us in order for the downstream firmware
     * to enable SPI reception and pre-load the first byte to be
     * transmitted.
     *
     * Similar delays are not uncommon for things like high-precision
     * delta-sigma A-to-D converters where the falling chip select
     * wakes the device from a low-power state, starts a conversion,
     * and can return data after some set delay.
     */
    for (i = 0; i < 25; i++);

    /*
     * Transmit the first byte, then go into EM1.  The IRQ handler will
     * receive each incoming byte and transmit the next outgoing byte.
     */
    USART0->TXDATA = outbuf[bufpos];

    while (bufpos < BUFLEN)
      EMU_EnterEM1();

    // De-assert chip select upon transfer completion (drive high)
    GPIO_PinOutSet(US0CS_PORT, US0CS_PIN);

    /*
     * Insert a short delay after CS de-assertion.  When this code is
     * running on a faster main (e.g. either higher clock rate or a
     * faster CPU such as a Cortex-M3), the secondary implementation
     * running on a slower device (e.g. either lower clock rate or a
     * slower CPU such as a Cortex-M0+) needs extra time to prepare
     * the input and output buffers for the next round of bytes to be
     * transferred.
     */
    for (i = 0; i < 20; i++);
  }
}
