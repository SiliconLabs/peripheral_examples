/***************************************************************************//**
 * @file main_xg27.c
 * @brief This project demonstrates low-frequency operation of the EUSART
 * using LDMA to receive inbound data and transmit outbound data while
 * remaining in EM2.
 *
 * After initialization, the MCU goes into EM2 where the receive LDMA
 * channel buffers the specified number of bytes of incoming data.  The
 * LDMA channel done interrupt wakes the device from EM2.  The CPU starts
 * the transmit channel and re-enters EM2.  The LDMA services the EUSART
 * transmit FIFO until the specified number of bytes are sent.  The LDMA
 * channel done interrupt again wakes the system. and the process repeats.
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

// MX25 driver to place SPI flash in shutdown mode
#include "mx25flash_spi.h"

// BSP for board controller pin macros
#include "bsp.h"

// Size of the buffer for received data
#define BUFLEN  10

// Receive data buffer
uint8_t buffer[BUFLEN];

// In low-frequency mode, the maximum EUSART baud rate is 9600
#define BAUDRATE             9600

// LDMA channels for receive and transmit servicing
#define RX_LDMA_CHANNEL 0
#define TX_LDMA_CHANNEL 1

// LDMA descriptor and transfer configuration structures for TX channel
LDMA_Descriptor_t ldmaTXDescriptor;
LDMA_TransferCfg_t ldmaTXConfig;

// LDMA descriptor and transfer configuration structures for RX channel
LDMA_Descriptor_t ldmaRXDescriptor;
LDMA_TransferCfg_t ldmaRXConfig;

/**************************************************************************//**
 * @brief
 *    Clock selection and initialization
 *****************************************************************************/
void initCMU(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

  // Select LFXO for the EUSART
  CMU_LFXOInit(&lfxoInit);
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
  CMU_ClockSelectSet(cmuClock_EUSART0, cmuSelect_EM23GRPACLK);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure the EUSART TX pin to the board controller as an output
  GPIO_PinModeSet(BSP_BCC_TXPORT, BSP_BCC_TXPIN, gpioModePushPull, 1);

  // Configure the EUSART RX pin to the board controller as an input
  GPIO_PinModeSet(BSP_BCC_RXPORT, BSP_BCC_RXPIN, gpioModeInput, 0);

  // Route EUSART0 TX and RX to the board controller TX and RX pins
  GPIO->EUSARTROUTE[0].TXROUTE = (BSP_BCC_TXPORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (BSP_BCC_TXPIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[0].RXROUTE = (BSP_BCC_RXPORT << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
      | (BSP_BCC_RXPIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);

  // Enable RX and TX signals now that they have been routed
  GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_RXPEN |
                                 GPIO_EUSART_ROUTEEN_TXPEN;

  /*
   * Configure the BCC_ENABLE pin as output and set high.  This enables
   * the virtual COM port (VCOM) connection to the board controller and
   * permits serial port traffic over the debug connection to the host
   * PC.
   *
   * To disable the VCOM connection and use the pins on the kit
   * expansion (EXP) header, comment out the following line.
   */
  GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief
 *    EUSART initialization
 *****************************************************************************/
void initEUSART0(void)
{
  CMU_ClockEnable(cmuClock_EUSART0, true);

  // Initialize the EUSART0 module
  EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_LF;
  EUSART_AdvancedInit_TypeDef advance_init = EUSART_ADVANCED_INIT_DEFAULT;

  init.baudrate = BAUDRATE;
  init.advancedSettings = &advance_init;
  init.advancedSettings->dmaWakeUpOnRx = true;
  init.advancedSettings->dmaWakeUpOnTx = true;
  init.advancedSettings->dmaHaltOnError = true;

  // Configure and enable EUSART0 for low-frequency (EM2) operation
  EUSART_UartInitLf(EUSART0, &init);
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

  // Source is buffer, destination is EUSART0_TXDATA, and length if BUFLEN
  ldmaTXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(buffer, &(EUSART0->TXDATA), BUFLEN);

  // Transfer a byte on free space in the EUSART FIFO
  ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_EUSART0_TXFL);

  // Source is EUSART0_RXDATA, destination is buffer, and length if BUFLEN
  ldmaRXDescriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(EUSART0->RXDATA), buffer, BUFLEN);

  // Transfer a byte on receive FIFO level event
  ldmaRXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_EUSART0_RXFL);
}

/**************************************************************************//**
 * @brief LDMA IRQHandler
 *****************************************************************************/
void LDMA_IRQHandler()
{
  uint32_t flags = LDMA_IntGet();

  // Clear the transmit channel's done flag if set
  if (flags & (1 << TX_LDMA_CHANNEL)) {
    LDMA_IntClear(1 << TX_LDMA_CHANNEL);
  }

  /*
   * Clear the receive channel's done flag if set and change receive
   * state to done.
   */
  if (flags & (1 << RX_LDMA_CHANNEL)) {
    LDMA_IntClear(1 << RX_LDMA_CHANNEL);
  }

  // Stop in case there was an error
  if (flags & LDMA_IF_ERROR) {
    __BKPT(0);
  }
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

  // Turn on DCDC regulator
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  /*
   * When developing or debugging code that enters EM2 or
   * lower, it's a good idea to have an "escape hatch" type
   * mechanism, e.g. a way to pause the device so that a debugger can
   * connect in order to erase flash, among other things.
   *
   * Before proceeding with this example, make sure PB0 is not pressed.
   * If the PB0 pin is low, turn on LED0 and execute the breakpoint
   * instruction to stop the processor in EM0 and allow a debug
   * connection to be made.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);

  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0) {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else {
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);
  }

  // Initialize and power-down MX25 SPI flash
  FlashStatus status;
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  // Initialize Clock, GPIO, EUSART and LDMA
  initCMU();
  initGPIO();
  initEUSART0();
  initLDMA();

  while (1)
  {
    // Zero out buffer
    for (i = 0; i < BUFLEN; i++)
      buffer[i] = 0;

    // Start the LDMA receive channel
    LDMA_StartTransfer(RX_LDMA_CHANNEL, &ldmaRXConfig, &ldmaRXDescriptor);

    // Wait in EM2 while receiving data
    EMU_EnterEM2(true);

    // start the LDMA transmit channel
    LDMA_StartTransfer(TX_LDMA_CHANNEL, &ldmaTXConfig, &ldmaTXDescriptor);

    // Wait in EM2 while transmitting data
    EMU_EnterEM2(true);
  }
}
