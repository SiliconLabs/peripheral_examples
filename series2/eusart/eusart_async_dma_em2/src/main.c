/***************************************************************************//**
 * @file main.c
 * @brief This project shows how to configure the EUSART for using LDMA to write
 * and read to the EUSART transmit/receive registers while remaining in EM2. This
 * example receives input from the serial terminal device and echoes it back.
 * See the readme.txt for details.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_eusart.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "mx25flash_spi.h"
#include "bspconfig.h"

#define RETARGET_TXPORT      BSP_BCC_TXPORT      /* EUSART transmission port */
#define RETARGET_TXPIN       BSP_BCC_TXPIN       /* EUSART transmission pin */
#define RETARGET_RXPORT      BSP_BCC_RXPORT      /* EUSART reception port */
#define RETARGET_RXPIN       BSP_BCC_RXPIN       /* EUSART reception pin */
#define BAUDRATE             9600                /* EUSART baudrate */

/**************************************************************************//**
 * @brief
 *    Clock selection and initialization
 *****************************************************************************/
void initClock(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

  // Select LFXO for the EUSART
  CMU_LFXOInit(&lfxoInit);
  CMU_ClockSelectSet(cmuClock_EUSART0, cmuSelect_LFXO);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure the EUSART TX pin to the board controller as an output
  GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1);

  // Configure the EUSART RX pin to the board controller as an input
  GPIO_PinModeSet(RETARGET_RXPORT, RETARGET_RXPIN, gpioModeInput, 0);

  // Route EUSART1 TX and RX to the board controller TX and RX pins
  GPIO->EUSARTROUTE->TXROUTE = (RETARGET_TXPORT
                               << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
                              | (RETARGET_TXPIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE->RXROUTE = (RETARGET_RXPORT
                               << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
                              | (RETARGET_RXPIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);

  // Enable RX and TX signals now that they have been routed
  GPIO->EUSARTROUTE->ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN|GPIO_EUSART_ROUTEEN_RXPEN;
}

/**************************************************************************//**
 * @brief
 *    EUSART initialization
 *****************************************************************************/
void initEusart0(void)
{
  // Enable EUSART0 Clock
  CMU_ClockEnable(cmuClock_EUSART0, true);

  // Initialize the EUSART0 module
  EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_LF;
  EUSART_AdvancedInit_TypeDef advance_init = EUSART_ADVANCED_INIT_DEFAULT;
  init.baudrate = BAUDRATE;
  init.advancedSettings = &advance_init;
  init.advancedSettings->dmaWakeUpOnRx = true;
  init.advancedSettings->dmaWakeUpOnTx = false;
  init.advancedSettings->dmaHaltOnError = true;

  // Configure and enable EUSART0 for low-frequency (EM2) operation
  EUSART_UartInitLf(EUSART0, &init);
}

/**************************************************************************//**
 * @brief
 *    LDMA initialization
 *****************************************************************************/
void initLdma(void)
{
  // Enable LDMA Clock
  CMU_ClockEnable(cmuClock_LDMA, true);

  // Channel descriptor configuration
  static LDMA_Descriptor_t descriptor =
    LDMA_DESCRIPTOR_SINGLE_P2P_BYTE(&(EUSART0->RXDATA), // Peripheral source address
                                    &(EUSART0->TXDATA), // Peripheral destination address
                                    4);               // Number of bytes per transfer
  descriptor.xfer.doneIfs  = 0;               // Don't trigger interrupt when done
  descriptor.xfer.linkMode = ldmaLinkModeRel; // Relative addressing to get next link
  descriptor.xfer.link     = 1;               // Enable descriptor linking
  descriptor.xfer.linkAddr = 0;               // Link to same descriptor

  // Transfer configuration and trigger selection
  LDMA_TransferCfg_t transferConfig =
    LDMA_TRANSFER_CFG_PERIPHERAL(LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0RXFL | 
                                            LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART0);

  // LDMA initialization
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Start the transfer
  uint32_t channelNum = 0;
  LDMA_StartTransfer(channelNum, &transferConfig, &descriptor);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Turn on DCDC regulator
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  EMU_UnlatchPinRetention();

  /*
   * When developing or debugging code that enters EM2 or
   *  lower, it's a good idea to have an "escape hatch" type
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
  initClock();
  initGpio();
  initEusart0();
  initLdma();

  while (1) {
    // Won't exit EM2
    EMU_EnterEM2(false);
  }
}