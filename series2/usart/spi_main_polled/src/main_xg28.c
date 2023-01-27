/***************************************************************************//**
 * @file main_xg28.c
 *
 * @brief This project demonstrates synchronous (SPI) use of the USART in polled
 * main mode. The main loop transmits the specified number of bytes and
 * receives the byte that is shifted in with each outgoing one.
 *
 * The pins used in this example are defined below and are described in the
 * accompanying readme.txt file.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

// Size of the data buffers
#define BUFLEN  10

#include "bsp.h"

// SPI ports and pins
#define US0MOSI_PORT  gpioPortD
#define US0MOSI_PIN   7
#define US0MISO_PORT  gpioPortD
#define US0MISO_PIN   8
#define US0CLK_PORT   gpioPortD
#define US0CLK_PIN    9
#define US0CS_PORT    gpioPortD
#define US0CS_PIN     10

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TX pin as an output
  GPIO_PinModeSet(US0MOSI_PORT, US0MOSI_PIN, gpioModePushPull, 0);

  // Configure RX pin as an input
  GPIO_PinModeSet(US0MISO_PORT, US0MISO_PIN, gpioModeInput, 0);

  // Configure CLK pin as an output low (CPOL = 0)
  GPIO_PinModeSet(US0CLK_PORT, US0CLK_PIN, gpioModePushPull, 0);

  // Configure CS pin as an output and drive inactive high
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
  CMU_ClockEnable(cmuClock_USART0, true);

  // Default asynchronous initializer (main mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  init.msbf = true;   // MSB first transmission for SPI compatibility

  /*
   * Route USART0 RX, TX, and CLK to the specified pins.  Note that CS is
   * not controlled by USART0 so there is no write to the corresponding
   * USARTROUTE register to do this.
   */
  GPIO->USARTROUTE[0].TXROUTE = (US0MOSI_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (US0MOSI_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].RXROUTE = (US0MISO_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (US0MISO_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CLKROUTE = (US0CLK_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (US0CLK_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN |    // MISO
                                GPIO_USART_ROUTEEN_TXPEN |    // MOSI
                                GPIO_USART_ROUTEEN_CLKPEN;

  // Configure and enable USART0
  USART_InitSync(USART0, &init);
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
 *    Main function
 *****************************************************************************/
int main(void)
{
  uint32_t i;

  // Outgoing data
  uint8_t outbuf[BUFLEN];

  // Incoming data
  uint8_t inbuf[BUFLEN];
  
  /*
   * Eliminate unused variable warning so that inbuf can be observed
   * in the debugger.
   */
  (void)inbuf;

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
     * Repeatedly perform single byte SPI transfers (transmission and
     * reception) of the specified length.  USART_SpiTransfer() polls
     * USART_STATUS_TXC for transmission complete, so this function ties
     * up the CPU until the last bit of the byte being transmitted is sent.
     */
    for (i = 0; i < BUFLEN; i++)
      inbuf[i] = USART_SpiTransfer(USART0, outbuf[i]);

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
