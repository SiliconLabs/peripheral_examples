/**************************************************************************//**
 * @file
 * @brief DMA transfers looped infinitely by consecutive software requests
 * @version 1.0.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_dma.h"

#include "dmactrl.h"

// DMA channel used
#define DMA_CHANNEL         0
#define DMA_CH_MASK         1 << DMA_CHANNEL

// Transfer buffer size
#define BUFFER_SIZE         8
#define TRANSFER_SIZE       (BUFFER_SIZE - 1)

#define ARB_SIZE            dmaArbitrate8

// This needs to agree with the datatype of the data transfered
#define DATA_SIZE           dmaDataSize2

// Buffer for memory to memory transfer
uint16_t srcBuffer[BUFFER_SIZE];
uint16_t dstBuffer[BUFFER_SIZE];

DMA_CB_TypeDef dmacb;

/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  uint32_t i;

  for(i = 0; i < BUFFER_SIZE; i++){
    srcBuffer[i]++;
  }

  // Request via software
  DMA->CHSWREQ |= DMA_CH_MASK;
}

/**************************************************************************//**
 * @brief DMA initialization
 *****************************************************************************/
void initDma(void)
{
  uint32_t i;

  // Initialize buffers for memory transfer
  for (i = 0; i < BUFFER_SIZE; i++){
    srcBuffer[i] = 1;
    dstBuffer[i] = 0;
  }

  // Enable CMU clock
  CMU_ClockEnable(cmuClock_DMA, true);

  // Basic DMA configuration
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;

  // Use primary channel
  bool isPrimary = true;

  // Configure general DMA issues
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;

  DMA_Init(&dmaInit);

  // Setup callback function
  dmacb.cbFunc  = dmaCallback;
  dmacb.userPtr = NULL;

  // Configure DMA channel used
  chnlCfg.highPri   = false;
  chnlCfg.enableInt = true;
  chnlCfg.select    = 0;          // software triggered
  chnlCfg.cb        = &dmacb;
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

  descrCfg.dstInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.srcInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.size    = DATA_SIZE;
  descrCfg.arbRate = ARB_SIZE;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(DMA_CHANNEL, isPrimary, &descrCfg);

  // Configure loop transfer mode
  DMA_CfgLoop_TypeDef loopConfig;
  loopConfig.enable  = true;                // Enable looping
  loopConfig.nMinus1 = TRANSFER_SIZE;       // Reload value for number of DMA transfers minus 1
  DMA_CfgLoop(DMA_CHANNEL, &loopConfig);

  // Start DMA
  DMA_ActivateBasic(
      DMA_CHANNEL,        // Channel
      isPrimary,
      false,
      dstBuffer,          // Destination
      srcBuffer,          // Source
      TRANSFER_SIZE);     // Transfer size

  // Request via software
  DMA->CHSWREQ |= DMA_CH_MASK;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialize DMA
  initDma();

  while (1)
  {
    EMU_EnterEM2(false);
  }
}
