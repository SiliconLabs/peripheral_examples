/***************************************************************************//**
 * @file main.c
 * @brief This project shows how to configure the EUART for half-duplex
 * communication. See the readme.txt for details.
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
#include "em_usart.h"
#include "mx25flash_spi.h"
#include "bspconfig.h"

// Size of the buffer for received data
#define BUFLEN  80

// Receive data buffer
char buffer[BUFLEN];

// Current position ins buffer
uint32_t inpos;
uint32_t outpos;

#define RETARGET_TXPORT      BSP_BCC_TXPORT      /* EUSART transmission port */
#define RETARGET_TXPIN       BSP_BCC_TXPIN       /* EUSART transmission pin */
#define RETARGET_RXPORT      BSP_BCC_RXPORT      /* EUSART reception port */
#define RETARGET_RXPIN       BSP_BCC_RXPIN       /* EUSART reception pin */
#define BAUDRATE             9600                /* EUSART baudrate */

#define INITIAL_TRANSMITTER
//#define INITIAL_RECEIVER

#if defined INITIAL_TRANSMITTER
  static char txSuccessMsg[] = "Initial TX: Receive success and transmitting now\r\n";
  bool receive = false;
#elif defined INITIAL_RECEIVER
  static char txSuccessMsg[] = "Initial RX: Receive success and transmitting now\r\n";
  bool receive = true;
#endif

/**************************************************************************//**
 * @brief
 *    The EUSART0 receive interrupt saves incoming characters.
 *****************************************************************************/
void EUSART0_RX_IRQHandler(void)
{
  // Get the character just received
  buffer[inpos] = EUSART_Rx(EUSART0);

  // Exit loop on new line or buffer full
  if ((buffer[inpos] != '\n') && (inpos < BUFLEN)) {
    inpos++;
  }
  else {
    receive = false;   // Stop receiving on CR
    EUSART_IntDisable(EUSART0, EUSART_IEN_RXFL);
  }

  // Clear the requesting interrupt before exiting the handler
  EUSART_IntClear(EUSART0, EUSART_IF_RXFL);
}

/**************************************************************************//**
 * @brief
 *    The EUSART0 transmit interrupt outputs characters.
 *****************************************************************************/
void EUSART0_TX_IRQHandler(void)
{
  // Send a previously received character
  if (outpos<inpos) {
    EUSART_Tx(EUSART0,buffer[outpos++]);
  }
  else {
  /*
   * Need to disable the transmit buffer level interrupt in this IRQ
   * handler when done or it will immediately trigger again upon exit
   * even though there is no data left to send.
   */
    receive = true;   // Go back into receive when all is sent
    EUSART_IntDisable(EUSART0, EUSART_IEN_TXC);
  }
  
  // Clear the requesting interrupt before exiting the handler
  EUSART_IntClear(EUSART0, EUSART_IF_TXC);
}

/**************************************************************************//**
 * @brief
 *    Clock selection and initialization
 *****************************************************************************/
void initClock(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

  // Select LFXO for the EUART
  CMU_LFXOInit(&lfxoInit);
  CMU_ClockSelectSet(cmuClock_EUSART0CLK, cmuSelect_LFXO);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure the EUSART TX pin to the board controller as an output
  GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1);

  // Route EUSART1 TX to the board controller TX pins
  GPIO->EUSARTROUTE[0].TXROUTE = (RETARGET_TXPORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (RETARGET_TXPIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);

  // Enable RX and TX signals now that they have been routed
  GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN;
}

/**************************************************************************//**
 * @brief
 *    EUART initialization
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
  init.advancedSettings->txAutoTristate = true;
  init.loopbackEnable = eusartLoopbackEnable;
  init.enable = eusartEnable;
  EUSART0->TIMINGCFG = EUSART_TIMINGCFG_TXDELAY_TRIPPLE;
  EUSART_UartInitLf(EUSART0, &init);

  // Enable NVIC USART sources
  NVIC_ClearPendingIRQ(EUSART0_RX_IRQn);
  NVIC_EnableIRQ(EUSART0_RX_IRQn);
  NVIC_ClearPendingIRQ(EUSART0_TX_IRQn);
  NVIC_EnableIRQ(EUSART0_TX_IRQn);
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

  // Initialize Clocks, GPIO and EUSART
  initClock();
  initGpio();
  initEusart0();

  while (1) {
    // Switch to RX
    EUSART_Enable(EUSART0, eusartEnableRx);
    EUSART_IntEnable(EUSART0, EUSART_IEN_RXFL);

    // Wait in EM2 while receiving to reduce current draw
    while (receive) {
      EMU_EnterEM2(false);
    }

    inpos = 0;
    for (uint32_t i = 0 ; txSuccessMsg[i]!=0; i++) {
      buffer[i] = txSuccessMsg[i];
      ++inpos;
    }

    // Switch to TX
    EUSART_Enable(EUSART0, eusartEnableTx);
    EUSART_IntEnable(EUSART0, EUSART_IEN_TXC);
    EUSART_IntSet(EUSART0, EUSART_IF_TXC);

    // Wait in EM1 while transmitting to reduce current draw
    while (!receive) {
      EMU_EnterEM1();
    }

    // Reset buffer indices
    inpos = outpos = 0;
    for (volatile uint32_t i = 0; i < BUFLEN; i++) {
      buffer[i] = 0;
    }
  }
}
