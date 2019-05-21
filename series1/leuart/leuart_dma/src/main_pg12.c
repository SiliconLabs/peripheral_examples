/**************************************************************************//**
 * @file main_pg12.c
 * @brief This project shows how to configure the LEUART for using DMA to write
 * and read to the LEUART transmit/receive registers while remaining in EM2.
 * This example receives input from the serial terminal device and echoes it
 * back. See the readme.txt for details.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_leuart.h"
#include "em_chip.h"
#include "em_ldma.h"

/**************************************************************************//**
 * @brief
 *    Initialize the GPIO pins for the LEUART module
 *****************************************************************************/
void initGpio(void)
{
  // GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize LEUART0 TX and RX pins
  GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 1); // TX
  GPIO_PinModeSet(gpioPortD, 11, gpioModeInput, 0);    // RX
}

/**************************************************************************//**
 * @brief
 *    Initialize the LEUART module
 *****************************************************************************/
void initLeuart(void)
{
  // Enable LE (low energy) clocks
  CMU_ClockEnable(cmuClock_HFLE, true); // Necessary for accessing LE modules
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO); // Set a reference clock

  // Enable clocks for LEUART0
  CMU_ClockEnable(cmuClock_LEUART0, true);
  CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1); // Don't prescale LEUART clock

  // Initialize the LEUART0 module
  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;
  LEUART_Init(LEUART0, &init);

  // Auto wake up DMA when data is received
  LEUART_RxDmaInEM2Enable(LEUART0, true);

  // Enable LEUART0 RX/TX pins on PD[11:10] (see readme.txt for details)
  LEUART0->ROUTELOC0 = LEUART_ROUTELOC0_RXLOC_LOC18 | LEUART_ROUTELOC0_TXLOC_LOC18;
  LEUART0->ROUTEPEN  = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
}

/**************************************************************************//**
 * @brief
 *    Initialize the DMA module
 *
 * @details
 *    Configure the channel descriptor to use the default peripheral to
 *    peripheral transfer descriptor. Modify it to not generate an interrupt
 *    upon transfer completion (we want to stay in EM2 mode the whole time).
 *    Also make the descriptor link to itself so that the descriptor runs
 *    continuously. Additionally, the transfer configuration selects the
 *    RXDATAV signal as the trigger for the DMA transaction to occur (i.e. when
 *    there is data in the LEUART0 receive buffer, transfer data from the RXDATA
 *    register to the TXDATA register) (this is the same as echoing the data
 *    back).
 *
 * @note
 *    The descriptor object needs to at least have static scope persistence so
 *    that the reference to the object is valid beyond its first use in
 *    initialization. This is because this code loops back to the same
 *    descriptor after every dma transfer. If the reference isn't valid anymore,
 *    then all dma transfers after the first one will fail. Oddly enough, this
 *    bug was noticed with no optimizations (gcc -O0) but not with (gcc -O2).
 *****************************************************************************/
void initDma(void)
{
  // Channel descriptor configuration
  static LDMA_Descriptor_t descriptor =
    LDMA_DESCRIPTOR_SINGLE_P2P_BYTE(&LEUART0->RXDATA, // Peripheral source address
                                    &LEUART0->TXDATA, // Peripheral destination address
                                    1);               // Number of bytes per transfer
  descriptor.xfer.doneIfs  = 0;               // Don't trigger interrupt when done
  descriptor.xfer.linkMode = ldmaLinkModeRel; // Relative addressing to get next link
  descriptor.xfer.link     = 1;               // Enable descriptor linking
  descriptor.xfer.linkAddr = 0;               // Link to same descriptor

  // Transfer configuration and trigger selection
  LDMA_TransferCfg_t transferConfig =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_LEUART0_RXDATAV);

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
 *
 * @details
 *    Initialize the GPIO, LEUART, and DMA modules. Go into EM2 forever and let
 *    the DMA take care of the data receiving/sending.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initGpio();
  initLeuart();
  initDma();

  while (1) {
    EMU_EnterEM2(false); // Won't exit EM2
  }
}

