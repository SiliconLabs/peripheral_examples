/**************************************************************************//**
 * @file main_series0.c
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
#include "em_dma.h"
#include "dmactrl.h"

/**************************************************************************//**
 * @brief
 *    Initialize the GPIO pins for the LEUART module
 *****************************************************************************/
void initGpio(void)
{
  // GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize LEUART0 TX and RX pins
  GPIO_PinModeSet(gpioPortD, 4, gpioModePushPull, 1); // TX
  GPIO_PinModeSet(gpioPortD, 5, gpioModeInput, 0);    // RX
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

  // Enable LEUART0 RX/TX pins on PD[5:4] (see readme.txt for details)
  LEUART0->ROUTE = LEUART_ROUTE_LOCATION_LOC0 | LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN;
}

/**************************************************************************//**
 * @brief
 *    Initialize the DMA module
 *
 * @details
 *    Always use dmaControlBlock to make sure that the control block is properly
 *    aligned. Tell the DMA module to trigger when there is data in the LEUART
 *    RX buffer. Don't trigger an interrupt upon transfer completion so that we
 *    stay in EM2 the entire time. This example chose to use channel 0.
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
  // Initializing the DMA
  DMA_Init_TypeDef init;
  init.hprot = 0; // Access level/protection not an issue
  init.controlBlock = dmaControlBlock; // Make sure control block is properly aligned
  DMA_Init(&init);

  // Channel configuration
  DMA_CfgChannel_TypeDef channelConfig;
  channelConfig.highPri   = false; // Set high priority for the channel
  channelConfig.enableInt = false; // Interrupt not needed in loop transfer mode
  channelConfig.select    = DMAREQ_LEUART0_RXDATAV; // Select DMA trigger
  channelConfig.cb        = NULL;                   // No callback because no interrupt
  uint32_t channelNum     = 0;
  DMA_CfgChannel(channelNum, &channelConfig);

  // Channel descriptor configuration
  static DMA_CfgDescr_TypeDef descriptorConfig;
  descriptorConfig.dstInc  = dmaDataIncNone; // Destination doesn't move
  descriptorConfig.srcInc  = dmaDataIncNone; // Source doesn't move
  descriptorConfig.size    = dmaDataSize1;   // Transfer 8 bits each time
  descriptorConfig.arbRate = dmaArbitrate1;  // Arbitrate after every DMA transfer
  descriptorConfig.hprot   = 0;              // Access level/protection not an issue
  bool isPrimaryDescriptor = true;
  DMA_CfgDescr(channelNum, isPrimaryDescriptor, &descriptorConfig);

  // Configure loop transfer mode
  DMA_CfgLoop_TypeDef loopConfig;
  loopConfig.enable  = true; // Enable looping
  loopConfig.nMinus1 = 0;    // Reload value for number of DMA transfers minus 1
  DMA_CfgLoop(channelNum, &loopConfig);

  // Activate basic DMA cycle (used for memory-peripheral transfers)
  bool isUseBurst = false;
  DMA_ActivateBasic(channelNum,
                    isPrimaryDescriptor,
                    isUseBurst,
                    (void *) &LEUART0->TXDATA, // Destination address to transfer to
                    (void *) &LEUART0->RXDATA, // Source address to transfer from
                    0);                        // Number of DMA transfers minus 1
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

  // Initialization
  initGpio();
  initLeuart();
  initDma();

  while (1) {
    EMU_EnterEM2(false); // Won't exit EM2
  }
}

