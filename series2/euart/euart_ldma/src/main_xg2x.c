/**************************************************************************//**
 * @main_xg2x.c
 * @brief This project shows how to configure the EUART for using LDMA to write
 * and read to the EUART transmit/receive registers while remaining in EM2.
 * This example receives input from the serial terminal device and echoes it
 * back. See the readme.txt for details.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2019 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
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

// Size of the buffer for received data
#define BUFLEN  90

// Receive data buffer
uint8_t buffer[BUFLEN];

// Current position ins buffer
uint32_t inpos = 0;
uint32_t outpos = 0;

// True while receiving data (waiting for CR or BUFLEN characters)
bool receive = true;

#define RETARGET_TXPORT      gpioPortA                    /* EUSART transmission port */
#define RETARGET_TXPIN       5                            /* EUSART transmission pin */
#define RETARGET_RXPORT      gpioPortA                    /* EUSART reception port */
#define RETARGET_RXPIN       6                            /* EUSART reception pin */
#define BAUDRATE             9600                         /* EUSART baudrate */

/**************************************************************************//**
 * @brief
 *    Clock selection and initialization
 *****************************************************************************/
void initClock(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

  // Select LFXO for the EUART
  CMU_LFXOInit(&lfxoInit);
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_EM23GRPACLK, true);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO Clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure GPIO pins
  // To avoid false start, configure output as high
  GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1);
  GPIO_PinModeSet(RETARGET_RXPORT, RETARGET_RXPIN, gpioModeInput, 0);

  GPIO->EUARTROUTE->ROUTEEN = GPIO_EUART_ROUTEEN_TXPEN;
  GPIO->EUARTROUTE->TXROUTE = (RETARGET_TXPORT
                               << _GPIO_EUART_TXROUTE_PORT_SHIFT)
                              | (RETARGET_TXPIN << _GPIO_EUART_TXROUTE_PIN_SHIFT);
  GPIO->EUARTROUTE->RXROUTE = (RETARGET_RXPORT
                               << _GPIO_EUART_RXROUTE_PORT_SHIFT)
                              | (RETARGET_RXPIN << _GPIO_EUART_RXROUTE_PIN_SHIFT);
}

/**************************************************************************//**
 * @brief
 *    EUART initialization
 *****************************************************************************/
void initEusart0(void)
{
  // Enable EUART0 Clock
  CMU_ClockSelectSet(cmuClock_EUART0, cmuSelect_EM23GRPACLK);
  CMU_ClockEnable(cmuClock_EUART0, true);

  // Initialize the EUART0 module
  EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_LF;
  EUSART_AdvancedInit_TypeDef advance_init = EUSART_ADVANCED_INIT_DEFAULT;
  init.baudrate = BAUDRATE;
  init.advancedSettings = &advance_init;
  init.advancedSettings->dmaWakeUpOnRx = true;
  init.advancedSettings->dmaWakeUpOnTx = false;
  init.advancedSettings->dmaHaltOnError = true;
  EUSART_UartInitLf(EUART0, &init);
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
    LDMA_DESCRIPTOR_SINGLE_P2P_BYTE(&(EUART0->RXDATA), // Peripheral source address
                                    &(EUART0->TXDATA), // Peripheral destination address
                                    4);               // Number of bytes per transfer
  descriptor.xfer.doneIfs  = 0;               // Don't trigger interrupt when done
  descriptor.xfer.linkMode = ldmaLinkModeRel; // Relative addressing to get next link
  descriptor.xfer.link     = 1;               // Enable descriptor linking
  descriptor.xfer.linkAddr = 0;               // Link to same descriptor

  // Transfer configuration and trigger selection
  LDMA_TransferCfg_t transferConfig =
    LDMA_TRANSFER_CFG_PERIPHERAL(LDMAXBAR_CH_REQSEL_SIGSEL_EUART0RXFL|LDMAXBAR_CH_REQSEL_SOURCESEL_EUART0);

  // LDMA initialization
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Start the transfer
  uint32_t channelNum = 0;
  LDMA_StartTransfer(channelNum, &transferConfig, &descriptor);
}

/**************************************************************************//**
 * @brief	Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  
  /* Turn on DCDC regulator */
  EMU_DCDCInit(&dcdcInit);

  EMU_UnlatchPinRetention();
  
  /* Init and power-down MX25 SPI flash */
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

  while (1){
    // Won't exit EM2
	EMU_EnterEM2(false);
  }
}