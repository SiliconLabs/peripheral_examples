/**************************************************************************//**
 * @file
 * @brief DMA transfer through a link list of descriptors.  This is achieved
 * through Ping-Pong mode with software intervention
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
#define ARB_SIZE            dmaArbitrate8

#define TRANSFER_SIZE       BUFFER_SIZE - 1

// Descriptor linked list size
#define LIST_SIZE           6

// Descriptor linked list for DMA transfer
DMA_CfgDescr_TypeDef        descrLink[LIST_SIZE];

// This needs to agree with the datatype of the data transfered
#define DATA_SIZE           dmaDataSize2

// Buffer for linked and looped transfer
uint16_t srcBuffer[LIST_SIZE][BUFFER_SIZE];
uint16_t dstBuffer[LIST_SIZE][BUFFER_SIZE];

DMA_CB_TypeDef dmacb;

uint32_t ppCnt;

/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  ppCnt++;

  // Stop after LIST_SIZE transfers
  if (ppCnt != LIST_SIZE){
    // Configure channel for next descriptor
    DMA_CfgDescr(DMA_CHANNEL, primary, &descrLink[ppCnt + 1]);

    // Enable next descriptor
    DMA_RefreshPingPong(
        channel,
        primary,
        false,
        dstBuffer[ppCnt + 1],
        srcBuffer[ppCnt + 1],
        TRANSFER_SIZE,
        false);                // Do not stop

    // Request next transfer via software
    DMA->CHSWREQ |= DMA_CH_MASK;
  }
}

/***************************************************************************//**
 * @brief
 *   Initialize the DMA controller for single direct register transfer
 ******************************************************************************/
void initDma(void)
{
  uint32_t i;

  ppCnt = 0;

  // Initialize buffers for memory transfer
  for (i = 0; i < BUFFER_SIZE * LIST_SIZE; i++){
    srcBuffer[i / BUFFER_SIZE][i % BUFFER_SIZE] = i;
    dstBuffer[i / BUFFER_SIZE][i % BUFFER_SIZE] = 0;
  }

  // Enable CMU clock
  CMU_ClockEnable(cmuClock_DMA, true);

  // Basic DMA configuration
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;

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
  chnlCfg.select    = 0;            // software triggered
  chnlCfg.cb        = &dmacb;
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

  descrCfg.dstInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.srcInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.size    = DATA_SIZE;
  descrCfg.arbRate = ARB_SIZE;
  descrCfg.hprot   = 0;

  // Set every descriptor in list to descrCfg
  for (i = 0; i < LIST_SIZE; i++){
    descrLink[i] = descrCfg;
  }

  DMA_CfgDescr(DMA_CHANNEL, true, &descrLink[0]);     // configure as primary
  DMA_CfgDescr(DMA_CHANNEL, false, &descrLink[1]);    // configure as alternate

  // Start DMA
  DMA_ActivatePingPong(
      DMA_CHANNEL,
      true,
      dstBuffer[0],         // primary destination
      srcBuffer[0],         // primary source
      TRANSFER_SIZE,
      dstBuffer[1],         // alternate destination
      srcBuffer[1],         // alternate source
      TRANSFER_SIZE);

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
