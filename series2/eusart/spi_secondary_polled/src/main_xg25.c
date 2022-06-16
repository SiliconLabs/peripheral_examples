/***************************************************************************//**
 * @file main_xg25.c
 *
 * @brief This project demonstrates polled use of the EUSART in synchronous
 * (SPI) secondary (formerly slave) mode. The main loop transmits the
 * specified number of bytes and receives the byte that is shifted in by the
 * main (formerly master) with each outgoing one.  The incoming data overwrites
 * the data buffer such that the initial transfer of data is echoed during the
 * next transfer.
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

// Size of the data buffers
#define BUFLEN  10

// SPI ports and pins
#define EUS1MOSI_PORT	gpioPortC
#define EUS1MOSI_PIN   	0
#define EUS1MISO_PORT  	gpioPortC
#define EUS1MISO_PIN   	1
#define EUS1SCLK_PORT  	gpioPortC
#define EUS1SCLK_PIN   	2
#define EUS1CS_PORT    	gpioPortB
#define EUS1CS_PIN     	4

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure MOSI (TX) pin as an input
  GPIO_PinModeSet(EUS1MOSI_PORT, EUS1MOSI_PIN, gpioModeInput, 0);

  // Configure MISO (RX) pin as an output
  GPIO_PinModeSet(EUS1MISO_PORT, EUS1MISO_PIN, gpioModePushPull, 0);

  // Configure SCLK pin as an input
  GPIO_PinModeSet(EUS1SCLK_PORT, EUS1SCLK_PIN, gpioModeInput, 0);

  // Configure CS pin as an input pulled high
  GPIO_PinModeSet(EUS1CS_PORT, EUS1CS_PIN, gpioModeInput, 1);

  /*
   * Configure CS pin for high-to-low edge detection, but don't enable
   * the interrupt.
   */
  GPIO_ExtIntConfig(EUS1CS_PORT, EUS1CS_PIN, EUS1CS_PIN, false, true, false);
}

/**************************************************************************//**
 * @brief
 *    EUSART1 initialization
 *****************************************************************************/
void initEUSART1(void)
{
  CMU_ClockEnable(cmuClock_EUSART1, true);

  // SPI advanced configuration (part of the initializer)
  EUSART_SpiAdvancedInit_TypeDef adv = EUSART_SPI_ADVANCED_INIT_DEFAULT;

  adv.msbFirst = true;        // SPI standard MSB first

  // Default asynchronous initializer (secondary/slave mode and 8-bit data)
  EUSART_SpiInit_TypeDef init = EUSART_SPI_SLAVE_INIT_DEFAULT_HF;

  init.advancedSettings = &adv;   // Advanced settings structure
  init.enable = eusartDisable;    // Don't enable yet

  // Route EUSART1 RX, TX, SCLK, and CS to the specified pins.
  GPIO->EUSARTROUTE[1].RXROUTE = (EUS1MISO_PORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
      | (EUS1MISO_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[1].TXROUTE = (EUS1MOSI_PORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (EUS1MOSI_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[1].SCLKROUTE = (EUS1SCLK_PORT << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
      | (EUS1SCLK_PIN << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[1].CSROUTE = (EUS1CS_PORT << _GPIO_EUSART_CSROUTE_PORT_SHIFT)
      | (EUS1CS_PIN << _GPIO_EUSART_CSROUTE_PIN_SHIFT);

  // Enable EUSART interface pins
  GPIO->EUSARTROUTE[1].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN   |
                                 GPIO_EUSART_ROUTEEN_TXPEN   |
                                 GPIO_EUSART_ROUTEEN_SCLKPEN |
                                 GPIO_EUSART_ROUTEEN_CSPEN;

  // Configure and enable EUSART1
  EUSART_SpiInit(EUSART1, &init);

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
  EUSART1->EN_CLR = EUSART_EN_EN;

  // Wait while the EUSART disables itself
  while ((EUSART1->EN & EUSART_EN_DISABLING));

  // Set FORCELOAD, then re-enable
  EUSART1->CFG2_SET = EUSART_CFG2_FORCELOAD;
  EUSART1->EN_SET = EUSART_EN_EN;
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

  // Zero buffer
  for (i = 0; i < BUFLEN; i++)
    buffer[i] = i;

  // Initialize GPIO and EUSART1
  initGPIO();
  initEUSART1();

  while (1)
  {
    // Clear the flag and wait for a falling edge on US0CS_PIN
    GPIO_IntClear(1 << EUS1CS_PIN);

    while ((GPIO_IntGet() & (1 << EUS1CS_PIN)) == 0);

    // Now enable the EUSART receiver and transmitter
    EUSART_Enable(EUSART1, eusartEnable);

    /*
     * Perform the specified number of single byte SPI transfers
     * (transmission and reception).
     *
     * Data to be queued for transmission is sent with EUSART_Tx(),
     * which checks that there is room in the transmit FIFO but does
     * not actually wait for transmit complete (EUSART_STATUS_TXC).
     *
     * Instead, incoming data is read from the FIFO with EUSART_Rx(),
     * which polls EUSART_STATUS_RXFL to make sure data has arrived.
     */
    for (i = 0; i < BUFLEN; i++)
    {
      EUSART_Tx(EUSART1, buffer[i]);
      buffer[i] = EUSART_Rx(EUSART1);
    }
  }
}
