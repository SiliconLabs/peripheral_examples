/***************************************************************************//**
 * @file main_xg27.c
 * @brief This project demonstrates use of the EUSART in single-wire,
 * half-duplex mode.  See the readme.txt for details.
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

// MAX25 driver to place SPI flash in shutdown mode
#include "mx25flash_spi.h"

// BSP for board pin macros
#include "bspconfig.h"

// Size of the buffer for received data
#define BUFLEN  80

// Receive data buffer
uint8_t buffer[BUFLEN];

// Current position in buffer
uint32_t bufpos;

// In low-frequency mode, the maximum EUSART baud rate is 9600
#define BAUDRATE             9600

/*
 * As explained in the readme.txt file, this example is designed to
 * run with one device being the initial transmitter and the other
 * being the initial receiver before switching roles.  Use the #define
 * below to select the operating state.
 */
 #define INITIAL_TRANSMITTER
 // #define INITIAL_RECEIVER

#if defined INITIAL_TRANSMITTER
  static uint8_t txSuccessMsg[] = "Initial TX: Receive success and transmitting now\r\n";
  bool receive = false;
#elif defined INITIAL_RECEIVER
  static uint8_t txSuccessMsg[] = "Initial RX: Receive success and transmitting now\r\n";
  bool receive = true;
#endif

/**************************************************************************//**
 * @brief
 *    Clock selection and initialization
 *****************************************************************************/
void initCMU(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

  // Select LFXO for the EUART
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

  // Route EUSART0 TX to the board controller TX pin
  GPIO->EUSARTROUTE[0].TXROUTE = (BSP_BCC_TXPORT << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (BSP_BCC_TXPIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);

  // Enable TX signal now that it has been routed
  GPIO->EUSARTROUTE[0].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN;

  /*
   * Configure the BCC_ENABLE pin as output and set high.  This enables
   * the virtual COM port (VCOM) connection to the board controller and
   * permits serial port traffic over the debug connection to the host
   * PC.
   *
   * To disable the VCOM connection and use only the pins on the kit
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

  // Initialize the EUSART0 for low-frequency operation
  EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_LF;
  EUSART_AdvancedInit_TypeDef advance_init = EUSART_ADVANCED_INIT_DEFAULT;

  init.baudrate = BAUDRATE;
  init.advancedSettings = &advance_init;
  init.advancedSettings->txAutoTristate = true;
  init.loopbackEnable = eusartLoopbackEnable;
  init.enable = eusartEnable;

  EUSART0->TIMINGCFG = EUSART_TIMINGCFG_TXDELAY_TRIPPLE;
  EUSART_UartInitLf(EUSART0, &init);

  // Enable NVIC EUSART sources
  NVIC_ClearPendingIRQ(EUSART0_RX_IRQn);
  NVIC_EnableIRQ(EUSART0_RX_IRQn);
  NVIC_ClearPendingIRQ(EUSART0_TX_IRQn);
  NVIC_EnableIRQ(EUSART0_TX_IRQn);
}

/**************************************************************************//**
 * @brief
 *    The EUSART0 receive interrupt saves incoming characters.
 *****************************************************************************/
void EUSART0_RX_IRQHandler(void)
{
  // Get the character just received
  buffer[bufpos] = EUSART_Rx(EUSART0);

  // Exit loop on new line or buffer full
  if ((buffer[bufpos] != '\n') && (bufpos < BUFLEN)) {
    bufpos++;
  }
  else {
    receive = false;   // Stop receiving on CR
    EUSART_IntDisable(EUSART0, EUSART_IEN_RXFL);
  }

  /*
   * The EUSART differs from the USART in that explicit clearing of
   * RX interrupt flags is required even after emptying the RX FIFO.
   */
  EUSART_IntClear(EUSART0, EUSART_IF_RXFL);
}

/**************************************************************************//**
 * @brief
 *    The EUSART0 transmit interrupt outputs characters.
 *****************************************************************************/
void EUSART0_TX_IRQHandler(void)
{
  // Stop sending when at string NULL terminator
  if (buffer[bufpos] != 0) {
    EUSART_Tx(EUSART0, buffer[bufpos++]);
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

  /*
   * The EUSART differs from the USART in that the TX complete
   * interrupt flag must be explicitly cleared even after a write to
   * the FIFO.
   */
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

  // Initialize Clocks, GPIO and EUSART
  initCMU();
  initGPIO();
  initEUSART0();

  while (1)
  {
    // Zero out buffer
    for (i = 0; i < BUFLEN; i++) {
      buffer[i] = 0;
    }

    // Reset buffer index
    bufpos = 0;

    /*
     * Enable receiving.  The eusartEnableRx enumeration enables the
     * receiver only, thus disabling the transmitter.
     */
    EUSART_Enable(EUSART0, eusartEnableRx);
    EUSART_IntEnable(EUSART0, EUSART_IEN_RXFL);

    // Wait in EM2 while receiving to reduce current draw
    while (receive) {
      EMU_EnterEM2(false);
    }

    // Copy the outbound message to the buffer
    for (i = 0 ; txSuccessMsg[i] != 0; i++) {
      buffer[i] = txSuccessMsg[i];
    }

    // Reset buffer index
    bufpos = 0;

    /*
     * Enable transmitting.  The eusartEnableTx enumeration enables the
     * transmitter only, thus disabling the receiver.  Setting the TXC
     * flag forces execution of the transmit of the transmit interrupt
     * handler, which sends the first outgoing byte.
     */
    EUSART_Enable(EUSART0, eusartEnableTx);
    EUSART_IntEnable(EUSART0, EUSART_IEN_TXC);
    EUSART_IntSet(EUSART0, EUSART_IF_TXC);

    // Wait in EM2 while transmitting to reduce current draw
    while (!receive) {
      EMU_EnterEM2(true);
    }
  }
}
