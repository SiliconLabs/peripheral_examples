/**************************************************************************//**
 * @main_series0.c
 * @brief This example shows how to use a timer and the DMA to output a
 * sinewave using the IDAC. This project operates in EM1.
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
#include "em_chip.h"
#include "em_idac.h"
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"

// Use a 32-point sine table
#define SINE_TABLE_SIZE 32

// Note: change this to choose the current range of the output
#define IDAC_RANGE IDAC_CURPROG_RANGESEL_RANGE3

// Note: change this to determine the frequency of the sine wave
#define WAVEFORM_FREQ 10000

// The timer needs to run at SINE_TABLE_SIZE times faster than the desired
// waveform frequency because it needs to output SINE_TABLE_SIZE points in that
// same amount of time
#define TIMER0_FREQ (WAVEFORM_FREQ * SINE_TABLE_SIZE)

// 32 point sine table with values shifted into place so that they can be
// directly written to the IDAC_CURPROG register. Only the values in the left
// most column (16, 19, 21, etc.) need to be changed
static const uint16_t sineTable[SINE_TABLE_SIZE] = {
  ((16 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((19 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((21 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((24 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((26 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((28 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((30 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((31 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((31 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((31 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((30 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((28 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((26 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((24 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((21 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((19 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((16 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((12 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((10 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((07 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((05 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((03 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((01 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((00 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((00 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((00 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((01 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((03 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((05 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((07 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((10 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
  ((12 << _IDAC_CURPROG_STEPSEL_SHIFT) | (IDAC_RANGE << _IDAC_CURPROG_RANGESEL_SHIFT)),
};

/***************************************************************************//**
* @brief
*   DMA callback function
*
* @details
*   This function gets called after the DMA transfer finishes. It then
*   refreshes the DMA transfer by activating another ping pong DMA cycle.
*
* @notes
*   The userPtr parameter is not used because we don't need a user defined
*   pointer. However, the function definition needs to have all three of
*   these parameters because the default handler provided in em_dma.c calls
*   the user's callback function this way. Alternatively, one could write
*   their own interrupt handler by defining the following macro:
*   EXCLUDE_DEFAULT_DMA_IRQ_HANDLER.
*
* @notes
*   isPrimaryDescriptor gets toggled in the default handler so that the
*   alternate and primary descriptors can get refreshed in sequential order
******************************************************************************/
void refreshTransfer(uint32_t channelNum,
                     bool isPrimaryDescriptor,
                     void *userPtr)
{
  DMA_RefreshPingPong(channelNum,
                      isPrimaryDescriptor,
                      false,                    // Don't use the burst feature
                      (void *) &IDAC0->CURPROG, // Destination address to transfer to
                      (void *) &sineTable[0],   // Source address to transfer from
                      SINE_TABLE_SIZE-1,        // Number of DMA transfers minus 1
                      false);                   // Don't stop after using this descriptor
}

/**************************************************************************//**
 * @brief
 *    Timer initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize TIMER0
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.enable = false;
  TIMER_Init(TIMER0, &init);

  // Set top (reload) value for the timer
  // Note: the timer runs off of the HFPER clock
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER) / TIMER0_FREQ;
  TIMER_TopBufSet(TIMER0, topValue);

  // Automatically clear the DMA request
  TIMER0->CTRL |= TIMER_CTRL_DMACLRACT;

  // Enable TIMER0
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    IDAC initialization
 *****************************************************************************/
void initIdac(void)
{
  // Enable IDAC clock
  CMU_ClockEnable(cmuClock_IDAC0, true);

  // Initialize IDAC
  IDAC_Init_TypeDef init = IDAC_INIT_DEFAULT;
  IDAC_Init(IDAC0, &init);

  // Enable IDAC output mode and also enable the IDAC module itself
  IDAC_OutEnable(IDAC0, true);
  IDAC_Enable(IDAC0, true);
}

/**************************************************************************//**
 * @brief
 *    Initialize the DMA module
 *
 * @details
 *    Always use dmaControlBlock to make sure that the control block is properly
 *    aligned. Tell the DMA module to trigger when the TIMER0 overflows (hits
 *    its TOP value and wraps around). Don't trigger an interrupt upon since we
 *    don't need it. This example chose to use channel 0
 *
 * @note
 *    The descriptor object and callback function need to at least have static
 *    scope persistence so that the reference to the object is valid beyond its
 *    first use in initialization. This is because this code loops back to the
 *    same descriptor after every dma transfer. If the reference isn't valid
 *    anymore, then all dma transfers after the first one will fail.
 ******************************************************************************/
void initDma(void)
{
  // Initializing the DMA
  DMA_Init_TypeDef init;
  init.hprot = 0; // Access level/protection not an issue
  init.controlBlock = dmaControlBlock; // Make sure control block is properly aligned
  DMA_Init(&init);

  // Callback configuration
  static DMA_CB_TypeDef callback;
  callback.cbFunc = (DMA_FuncPtr_TypeDef) refreshTransfer;
  callback.userPtr = NULL; // Unused

  // Channel configuration
  DMA_CfgChannel_TypeDef channelConfig;
  channelConfig.highPri   = false; // Set high priority for the channel
  channelConfig.enableInt = true;  // Interrupt needed because no support for loop mode
  channelConfig.select    = DMAREQ_TIMER0_UFOF; // Select DMA trigger
  channelConfig.cb        = &callback;          // Need callback to refresh DMA transfer
  uint32_t channelNum     = 0;
  DMA_CfgChannel(channelNum, &channelConfig);

  // Channel configuration for ping pong mode
  static DMA_CfgDescr_TypeDef descriptorConfig;
  descriptorConfig.dstInc  = dmaDataIncNone; // Destination doesn't move
  descriptorConfig.srcInc  = dmaDataInc2;    // Source moves 2 bytes each transfer
  descriptorConfig.size    = dmaDataSize2;   // Transfer 2 bytes each time
  descriptorConfig.arbRate = dmaArbitrate1;  // Arbitrate after every DMA transfer
  descriptorConfig.hprot   = 0;              // Access level/protection not an issue
  DMA_CfgDescr(channelNum, true, &descriptorConfig);  // Primary descriptor
  DMA_CfgDescr(channelNum, false, &descriptorConfig); // Alternate descriptor

  // Activate ping pong DMA cycle (used for memory-peripheral transfers)
  bool isUseBurst = false;
  DMA_ActivatePingPong(channelNum,
                       isUseBurst,
                       (void *) &IDAC0->CURPROG, // Primary destination address to transfer to
                       (void *) &sineTable[0],   // Primary source address to transfer from
                       SINE_TABLE_SIZE-1,        // Primary number of DMA transfers minus 1
                       (void *) &IDAC0->CURPROG, // Alternate destination address to transfer to
                       (void *) &sineTable[0],   // Alternate source address to transfer from
                       SINE_TABLE_SIZE-1);       // Alternate number of DMA transfers minus 1
}

/**************************************************************************//**
 * @brief
 *    Use a timer to trigger the DMA to output to the IDAC.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialization
  initIdac();
  initDma();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

