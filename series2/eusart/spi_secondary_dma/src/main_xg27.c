/***************************************************************************//**
 * @file main_xg27.c
 *
 * @brief This project demonstrates DMA-driven use of the EUSART in synchronous
 * (SPI) secondary (formerly slave) mode.  The main loop starts the LDMA
 * channels, which transmit the specified number of bytes and receive the byte
 * that is shifted in with each outgoing one.
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

// SPI ports and pins
#define EUS0MOSI_PORT   gpioPortC
#define EUS0MOSI_PIN    0
#define EUS0MISO_PORT   gpioPortC
#define EUS0MISO_PIN    1
#define EUS0SCLK_PORT   gpioPortC
#define EUS0SCLK_PIN    2
#define EUS0CS_PORT     gpioPortC
#define EUS0CS_PIN      3

// LDMA channel for receive and transmit servicing
#define RX_LDMA_CHANNEL 0
#define TX_LDMA_CHANNEL 1

// LDMA descriptor and transfer configuration structures for EUSART TX channel
LDMA_Descriptor_t ldmaTXDescriptor;
LDMA_TransferCfg_t ldmaTXConfig;

// LDMA descriptor and transfer configuration structures for EUSART RX channel
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

  // Enable EUSART interface pins
  GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN |    // MISO
                                 GPIO_EUSART_ROUTEEN_TXPEN |    // MOSI
                                 GPIO_EUSART_ROUTEEN_SCLKPEN |
                                 GPIO_EUSART_ROUTEEN_CSPEN;

  // Route EUSART0 RX, TX, CLK, and CS to the specified pins.
  GPIO->EUSARTROUTE[0].TXROUTE = (EUS0MOSI_PORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (EUS0MOSI_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].RXROUTE = (EUS0MISO_PORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
      | (EUS0MISO_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].SCLKROUTE = (EUS0SCLK_PORT << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
      | (EUS0SCLK_PIN << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].CSROUTE = (EUS0CS_PORT << _GPIO_EUSART_CSROUTE_PORT_SHIFT)
      | (EUS0CS_PIN << _GPIO_EUSART_CSROUTE_PIN_SHIFT);

  // Configure MOSI (TX) pin as an input
  GPIO_PinModeSet(EUS0MOSI_PORT, EUS0MOSI_PIN, gpioModeInput, 0);

  // Configure MISO (RX) pin as an output
  GPIO_PinModeSet(EUS0MISO_PORT, EUS0MISO_PIN, gpioModePushPull, 0);

  // Configure SCLK pin as an input
  GPIO_PinModeSet(EUS0SCLK_PORT, EUS0SCLK_PIN, gpioModeInput, 0);

  // Configure CS pin as an input pulled high
  GPIO_PinModeSet(EUS0CS_PORT, EUS0CS_PIN, gpioModeInputPull, 1);

  // Request an interrupt on a CS pin high-to-low transition.
  GPIO_ExtIntConfig(EUS0CS_PORT, EUS0CS_PIN, EUS0CS_PIN, false, true, true);

  // Enable NVIC GPIO interrupt
#if (EUS0CS_PIN & 1)
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
  EUSART_SpiInit_TypeDef init = EUSART_SPI_SLAVE_INIT_DEFAULT_HF;

  init.advancedSettings = &adv;   // Advanced settings structure
  init.enable = eusartDisable;    // Do not enable yet

  // Configure EUSART0
  EUSART_SpiInit(EUSART0, &init);

  /*
   * In polled mode, there should be a delay of reasonable length
   * between chip select assertion and when the main device begins
   * clocking the secondary.  This delay is needed to allow the
   * secondary to recognize the CSn falling edge in order to exit a
   * low-power mode (especially EM2/3) and to prepare data for
   * transmission back to the main.
   *
   * Unfortunately, the EUSART default behavior assumes that an
   * extended delay means that the secondary should transmit a default
   * frame back to the main, which is probably unnecessary.
   *
   * To avoid this, the FORCELOAD bit the in the CFG2 register must
   * be set, but this requires disabling the EUSART because the emlib
   * advanced initializer for SPI mode does not include an option to
   * set this (even though it includes a way to set the default frame).
   */
  EUSART0->EN_CLR = EUSART_EN_EN;

  // Wait while the EUSART disables itself
  while ((EUSART0->EN & EUSART_EN_DISABLING));

  // Set FORCELOAD, then re-enable
  EUSART0->CFG2_SET = EUSART_CFG2_FORCELOAD;
  EUSART0->EN_SET = EUSART_EN_EN;
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

  // Source is EUSART0_RXDATA, destination is inbuf, and length if BUFLEN
  ldmaRXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(EUSART0->RXDATA), inbuf, BUFLEN);

  // Transfer a byte on receive FIFO level event
  ldmaRXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_EUSART0_RXFL);

  // Source is outbuf, destination is EUSART0_TXDATA, and length if BUFLEN
  ldmaTXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(outbuf, &(EUSART0->TXDATA), BUFLEN);

  // Transfer a byte on free space in the EUSART FIFO
  ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_EUSART0_TXFL);
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
 * @brief GPIO IRQHandler
 *****************************************************************************/
#if (EUS0CS_PIN & 1)
void GPIO_ODD_IRQHandler(void)
#else
void GPIO_EVEN_IRQHandler(void)
#endif
{
  // Clear the falling edge interrupt flag
  GPIO_IntClear(1 << EUS0CS_PIN);
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

  // Run from the HFXO to maximize performance
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  CMU_HFXOInit(&hfxoInit);

  // Set DPLL to 80 MHz (40 MHz PCLK)
  CMU_DPLLInit_TypeDef initDPLL = CMU_DPLL_HFXO_TO_80MHZ;

  // Attempt DPLL lock; halt on failure
  if (CMU_DPLLLock(&initDPLL) == false)
    __BKPT(0);

  /*
   * Set the optimal number of wait states for the selected HFCLK
   * frequency.  Highest performance is achieved without voltage
   * scaling, which is specified by 0 for the second parameter.
   */
  CMU_UpdateWaitStates(SystemCoreClockGet(), 0);

  // Initialize EUSART0, GPIO, and LDMA
  initEUSART0();
  initGPIO();
  initLDMA();

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

    // Now enable the falling edge interrupt on the EUS0CS_PIN
    GPIO_IntEnable(1 << EUS0CS_PIN);

    // Wait for falling edge on EUS0CS_PIN
    EMU_EnterEM1();

    // Now enable the EUSART receiver and transmitter
    EUSART_Enable(EUSART0, eusartEnable);

    // Start both channels
    LDMA_StartTransfer(TX_LDMA_CHANNEL, &ldmaTXConfig, &ldmaTXDescriptor);
    LDMA_StartTransfer(RX_LDMA_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);

    // Wait in EM1 until all data is received
    while (!rx_done)
      EMU_EnterEM1();

    // Disable the falling edge interrupt on the EUS0CS_PIN
    GPIO_IntDisable(1 << EUS0CS_PIN);

    // Disable EUSART receiver and transmitter until next chip select
    EUSART_Enable(EUSART0, eusartDisable);
  }
}
