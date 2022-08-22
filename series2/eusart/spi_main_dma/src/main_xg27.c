/***************************************************************************//**
 * @file main_xg27.c
 *
 * @brief This project demonstrates DMA-driven use of the EUSART in synchronous
 * (SPI) main mode. The main loop starts the LDMA channels, which transmit the
 * specified number of bytes and receive the byte that is shifted in with each
 * outgoing one.
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
#include "em_ldma.h"

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

// LDMA channels for receive and transmit servicing
#define RX_LDMA_CHANNEL 0
#define TX_LDMA_CHANNEL 1

// LDMA descriptor and transfer configuration structures for TX channel
LDMA_Descriptor_t ldmaTXDescriptor;
LDMA_TransferCfg_t ldmaTXConfig;

// LDMA descriptor and transfer configuration structures for RX channel
LDMA_Descriptor_t ldmaRXDescriptor;
LDMA_TransferCfg_t ldmaRXConfig;

// Size of the data buffers
#define BUFLEN  10

// Outgoing data
uint8_t outbuf[BUFLEN];

// Incoming data
uint8_t inbuf[BUFLEN];

// Data reception complete
bool rx_done;

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
  adv.autoInterFrameTime = 7; // 7 bit times of delay between frames
                              // to accommodate non-DMA secondaries

  // Default asynchronous initializer (main/master mode and 8-bit data)
  EUSART_SpiInit_TypeDef init = EUSART_SPI_MASTER_INIT_DEFAULT_HF;

  init.bitRate = 1000000;         // 1 MHz shift clock
  init.advancedSettings = &adv;   // Advanced settings structure

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
}

/**************************************************************************//**
 * @brief
 *    LDMA initialization
 *****************************************************************************/
void initLDMA(void)
{
  // First, initialize the LDMA unit itself
  LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;
  LDMA_Init(&ldmaInit);

  // Source is outbuf, destination is EUSART0_TXDATA, and length if BUFLEN
  ldmaTXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(outbuf, &(EUSART0->TXDATA), BUFLEN);

  // Transfer a byte on free space in the EUSART FIFO
  ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_EUSART0_TXFL);

  // Source is EUSART0_RXDATA, destination is inbuf, and length if BUFLEN
  ldmaRXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(EUSART0->RXDATA), inbuf, BUFLEN);

  // Transfer a byte on receive FIFO level event
  ldmaRXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_EUSART0_RXFL);
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
 * @brief LDMA IRQHandler
 *****************************************************************************/
void LDMA_IRQHandler()
{
  uint32_t flags = LDMA_IntGet();

  // Clear the transmit channel's done flag if set
  if (flags & (1 << TX_LDMA_CHANNEL))
    LDMA_IntClear(1 << TX_LDMA_CHANNEL);

  /*
   * Clear the receive channel's done flag if set and change receive
   * state to done.
   */
  if (flags & (1 << RX_LDMA_CHANNEL))
  {
    LDMA_IntClear(1 << RX_LDMA_CHANNEL);
    rx_done = true;
  }

  // Stop in case there was an error
  if (flags & LDMA_IF_ERROR)
    __BKPT(0);
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
  initLDMA();

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

    // Set the receive state to not done
    rx_done = false;

    /*
     * Assert chip select (drive low).  Note that the calls to
     * LDMA_StartTransfer() for the receive and transmit channels take
     * about 42 microseconds at 19 MHz, which the secondary device
     * needs in order to service the GPIO falling edge interrupt on
     * CS and to prepare to receive the incoming data.
     */
    GPIO_PinOutClear(EUS0CS_PORT, EUS0CS_PIN);

    // Start both channels
    LDMA_StartTransfer(RX_LDMA_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);
    LDMA_StartTransfer(TX_LDMA_CHANNEL, &ldmaTXConfig, &ldmaTXDescriptor);

    // Wait in EM1 until all data is received
    while (!rx_done)
      EMU_EnterEM1();

    // De-assert chip select upon final byte reception (drive high)
    GPIO_PinOutSet(EUS0CS_PORT, EUS0CS_PIN);
  }
}
