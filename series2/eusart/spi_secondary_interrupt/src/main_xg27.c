/***************************************************************************//**
 * @file main_xg27.c
 *
 * @brief This project demonstrates interrupt-driven use of the EUSART in
 * synchronous (SPI) secondary (formerly slave) mode.  The main loop
 * transmits the specified number of bytes and receives the byte that is
 * shifted in by the main/master with each outgoing one.
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

// SPI ports and pins
#define EUS0MOSI_PORT   gpioPortC
#define EUS0MOSI_PIN    0
#define EUS0MISO_PORT   gpioPortC
#define EUS0MISO_PIN    1
#define EUS0SCLK_PORT   gpioPortC
#define EUS0SCLK_PIN    2
#define EUS0CS_PORT     gpioPortC
#define EUS0CS_PIN      3

/*
 * The TIMEPORT/TIMEPIN is not part of the SPI bus.  It shows when the
 * CPU responds to the main before, during, and after data transfer.
 * Use a logic analyzer to capture the activity on this pin along with
 * the bus traffic to understand the timing relationship between the
 * CPU and the SPI during interrupt-driven transfers.
 */
#define TIMEPORT      gpioPortB
#define TIMEPIN       1

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
  // Enable clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure MOSI (TX) pin as an input
  GPIO_PinModeSet(EUS0MOSI_PORT, EUS0MOSI_PIN, gpioModeInput, 0);

  // Configure MISO (RX) pin as an output
  GPIO_PinModeSet(EUS0MISO_PORT, EUS0MISO_PIN, gpioModePushPull, 0);

  // Configure SCLK pin as an input
  GPIO_PinModeSet(EUS0SCLK_PORT, EUS0SCLK_PIN, gpioModeInput, 0);

  // Configure CS pin as an input pulled high
  GPIO_PinModeSet(EUS0CS_PORT, EUS0CS_PIN, gpioModeInput, 1);

  // Generate an interrupt on a CS pin high-to-low transition.
  GPIO_ExtIntConfig(EUS0CS_PORT, EUS0CS_PIN, EUS0CS_PIN, false, true, false);

  // Enable NVIC GPIO interrupt
#if (EUS0CS_PIN & 1)
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
#else
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
#endif

  // Enable the activity pin
  GPIO_PinModeSet(TIMEPORT, TIMEPIN, gpioModePushPull, 0);
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

  // Default asynchronous initializer (secondary/slave mode and 8-bit data)
  EUSART_SpiInit_TypeDef init = EUSART_SPI_SLAVE_INIT_DEFAULT_HF;

  init.advancedSettings = &adv;   // Advanced settings structure
  init.enable = eusartDisable;    // Don't enable yet

  // Route EUSART0 RX, TX, SCLK, and CS to the specified pins.
  GPIO->EUSARTROUTE[0].RXROUTE = (EUS0MISO_PORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
      | (EUS0MISO_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].TXROUTE = (EUS0MOSI_PORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (EUS0MOSI_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].SCLKROUTE = (EUS0SCLK_PORT << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
      | (EUS0SCLK_PIN << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].CSROUTE = (EUS0CS_PORT << _GPIO_EUSART_CSROUTE_PORT_SHIFT)
      | (EUS0CS_PIN << _GPIO_EUSART_CSROUTE_PIN_SHIFT);

  // Enable EUSART interface pins
  GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN   |
                                 GPIO_EUSART_ROUTEEN_TXPEN   |
                                 GPIO_EUSART_ROUTEEN_SCLKPEN |
                                 GPIO_EUSART_ROUTEEN_CSPEN;

  // Configure and enable EUSART0
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

  // Enable NVIC EUSART sources
  NVIC_ClearPendingIRQ(EUSART0_RX_IRQn);
  NVIC_EnableIRQ(EUSART0_RX_IRQn);
  EUSART_IntClear(EUSART0, EUSART_IF_TXC);
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
 *    EUSART0 receive interrupt handler
 *****************************************************************************/
void EUSART0_RX_IRQHandler(void)
{
  // Drive the activity pin high to denote IRQ handler entry
  GPIO->P_SET[TIMEPORT].DOUT = 1 << TIMEPIN;

  /*
   * Save the byte received concurrent with the transmission of the
   * last bit of the previous outgoing byte, and increment the buffer
   * position to the next byte.  Note that this read clears the
   * EUSART_IF_RXFL interrupt flag.
   */
  inbuf[bufpos++] = EUSART0->RXDATA;

  // If there are still bytes left to send, transmit the next one
  if (bufpos < BUFLEN)
    EUSART0->TXDATA = (uint16_t)outbuf[bufpos];

  // Drive the activity pin low to denote IRQ handler exit
  GPIO->P_CLR[TIMEPORT].DOUT = 1 << TIMEPIN;

  // Clear the requesting interrupt before exiting the handler
  EUSART_IntClear(EUSART0, EUSART_IF_RXFL);
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
   * Set the optimal number of wait state for the selected HFCLK
   * frequency.  Note that the second parameter, which specifies the
   * voltage scaling level, is meaningless on Series 0 devices.
   */
  CMU_UpdateWaitStates(SystemCoreClockGet(), 0);

  // Initialize GPIO and EUSART0
  initGPIO();
  initEUSART0();

  // Drive the activity pin high to denote IRQ handler entry
  GPIO->P_SET[TIMEPORT].DOUT = 1 << TIMEPIN;

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

    // Enable the falling edge interrupt on the EUS0CS_PIN
    GPIO_IntClear(1 << EUS0CS_PIN);
    GPIO_IntEnable(1 << EUS0CS_PIN);

    // Drive the activity pin low when ready for CS assertion
    GPIO->P_CLR[TIMEPORT].DOUT = 1 << TIMEPIN;

    // Wait for falling edge on EUS0CS_PIN
    EMU_EnterEM1();

    /*
     * Drive the activity pin high on wake from EM1 immediately after
     * CS assertion
     */
    GPIO->P_SET[TIMEPORT].DOUT = 1 << TIMEPIN;

    // Now enable the EUSART receiver and transmitter
    EUSART_Enable(EUSART0, eusartEnable);

    // Enable receive data valid interrupt
    EUSART_IntEnable(EUSART0, EUSART_IEN_RXFL);

    /*
     * Transmit the first byte, then go into EM1.  The IRQ handler will
     * receive each incoming byte and transmit the next outgoing byte.
     */
    EUSART0->TXDATA = (uint16_t)outbuf[bufpos];

    // Drive the activity pin low when ready to receive data
    GPIO->P_CLR[TIMEPORT].DOUT = 1 << TIMEPIN;

    // Wait in EM1 until all data is received
    while (bufpos < BUFLEN)
      EMU_EnterEM1();

    // Drive the activity pin high to show prep for next data transfer
    GPIO->P_SET[TIMEPORT].DOUT = 1 << TIMEPIN;

    // Disable receive data interrupt
    EUSART_IntDisable(EUSART0, EUSART_IEN_RXFL);

    // Disable the falling edge interrupt on the EUS0CS_PIN
    GPIO_IntDisable(1 << EUS0CS_PIN);

    // Disable EUSART receiver and transmitter until next chip select
    EUSART_Enable(EUSART0, eusartDisable);
  }
}
