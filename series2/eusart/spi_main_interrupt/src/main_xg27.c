/***************************************************************************//**
 * @file main_xg27.c
 *
 * @brief This project demonstrates interrupt-driven use of the EUSART in
 * synchronous (SPI) main (formerly master) mode. The main loop transmits the
 * specified number of bytes and receives the byte that is shifted in with
 * each outgoing one.
 *
 * The pins used in this example are defined below and are described in the
 * accompanying readme.txt file.
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

#include "bsp.h"

// SPI ports and pins
#define EUS0MOSI_PORT   gpioPortC
#define EUS0MOSI_PIN    0
#define EUS0MISO_PORT   gpioPortC
#define EUS0MISO_PIN    1
#define EUS0SCLK_PORT   gpioPortC
#define EUS0SCLK_PIN    2
#define EUS0CS_PORT     gpioPortC
#define EUS0CS_PIN      3

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
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure MOSI (TX) pin as an output
  GPIO_PinModeSet(EUS0MOSI_PORT, EUS0MOSI_PIN, gpioModePushPull, 0);

  // Configure MISO (RX) pin as an input
  GPIO_PinModeSet(EUS0MISO_PORT, EUS0MISO_PIN, gpioModeInput, 0);

  // Configure SCLK pin as an output low (CPOL = 0)
  GPIO_PinModeSet(EUS0SCLK_PORT, EUS0SCLK_PIN, gpioModePushPull, 0);

  // Configure CS pin as an output initially high
  GPIO_PinModeSet(EUS0CS_PORT, EUS0CS_PIN, gpioModePushPull, 1);

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
 *    EUSART0 initialization
 *****************************************************************************/
void initEUSART0(void)
{
  CMU_ClockEnable(cmuClock_EUSART0, true);

  // SPI advanced configuration (part of the initializer)
  EUSART_SpiAdvancedInit_TypeDef adv = EUSART_SPI_ADVANCED_INIT_DEFAULT;

  adv.msbFirst = true;        // SPI standard MSB first

  // Default asynchronous initializer (main/master mode and 8-bit data)
  EUSART_SpiInit_TypeDef init = EUSART_SPI_MASTER_INIT_DEFAULT_HF;

  init.bitRate = 1000000;        // 1 MHz shift clock
  init.advancedSettings = &adv;  // Advanced settings structure

  /*
   * Route EUSART0 MOSI, MISO, and SCLK to the specified pins.  CS is
   * not controlled by EUSART0 so there is no write to the corresponding
   * EUSARTROUTE register to do this.
   */
  GPIO->EUSARTROUTE[0].TXROUTE = (EUS0MOSI_PORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (EUS0MOSI_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].RXROUTE = (EUS0MISO_PORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
      | (EUS0MISO_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].SCLKROUTE = (EUS0SCLK_PORT << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
      | (EUS0SCLK_PIN << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT);

  // Enable EUSART interface pins
  GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN |    // MISO
                                 GPIO_EUSART_ROUTEEN_TXPEN |    // MOSI
                                 GPIO_EUSART_ROUTEEN_SCLKPEN;

  // Configure and enable EUSART0
  EUSART_SpiInit(EUSART0, &init);

  // Clear and enable transmit complete interrupt
  EUSART_IntEnable(EUSART0, EUSART_IEN_TXC);
  EUSART_IntClear(EUSART0, EUSART_IF_TXC);

  // Enable NVIC EUSART source
  NVIC_ClearPendingIRQ(EUSART0_TX_IRQn);
  NVIC_EnableIRQ(EUSART0_TX_IRQn);
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
 *    EUSART0 transmit interrupt handler
 *****************************************************************************/
void EUSART0_TX_IRQHandler(void)
{
  /*
   * Save byte received concurrent with the transmission of the last bit of
   * the previous outgoing byte, and increment the buffer position to the
   * next byte.
   */
  inbuf[bufpos++] = EUSART0->RXDATA;

  // If there are still bytes left to send, transmit the next one
  if (bufpos < BUFLEN)
    EUSART0->TXDATA = outbuf[bufpos];

  // Clear the requesting interrupt before exiting the handler
  EUSART_IntClear(EUSART0, EUSART_IF_TXC);
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

  // Initialize DCDC with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialize GPIO and EUSART0
  initGPIO();
  initEUSART0();

  /*
   * Wait for button 0 press before starting.  This happens one time so
   * that the device running the matching secondary (formerly slave) code
   * can be started first, otherwise it would be difficult to keep
   * everything synchronized without a separate handshaking mechanism.
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
    GPIO_PinOutClear(EUS0CS_PORT, EUS0CS_PIN);

    /*
     * Because this example is most likely going to be running with
     * another EFM32/EFR32 device on the secondary side, it must
     * insert a delay between chip select assertion and sending the
     * first byte.
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
    EUSART0->TXDATA = outbuf[bufpos];

    while (bufpos < BUFLEN)
      EMU_EnterEM1();

    // De-assert chip select upon transfer completion (drive high)
    GPIO_PinOutSet(EUS0CS_PORT, EUS0CS_PIN);

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
