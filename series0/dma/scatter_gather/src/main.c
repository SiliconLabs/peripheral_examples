/**************************************************************************//**
 * @file
 * @brief DMA transfers of linked list via Scatter-Gather
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
#define DMA_CH_MASK         (1 << DMA_CHANNEL)

// Transfer buffer size and arbitration size (should agree)
#define BUFFER_SIZE         8
#define ARB_SIZE            dmaArbitrate8

#define TRANSFER_SIZE       BUFFER_SIZE - 1

// Descriptor linked list size
#define LIST_SIZE           4

// Descriptor linked list for DMA transfer
DMA_DESCRIPTOR_TypeDef      altDescrs[LIST_SIZE];

// Data type size of srcBuffer and dstBuffer
#define DATA_SIZE           dmaDataSize2

// Buffer for linked and looped transfer
uint16_t srcBuffer[LIST_SIZE * BUFFER_SIZE];
uint16_t dstBuffer[LIST_SIZE][BUFFER_SIZE];

DMA_CB_TypeDef dmacb;

/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  // Insert transfer complete functionality here
}

/***************************************************************************//**
 * @brief
 *   Descriptor linked list example.
 ******************************************************************************/
void initDma(void)
{
  uint32_t i;

  // Fill source buffer and clear destination buffer
  for (i = 0; i < BUFFER_SIZE * LIST_SIZE; i++){
    srcBuffer[i] = i;
    dstBuffer[i/BUFFER_SIZE][i%BUFFER_SIZE] = 0;
  }

  // Enable DMA clock
  CMU_ClockEnable(cmuClock_DMA, true);

  // Basic DMA configuration
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgChannel_TypeDef chnlCfg;
  DMA_CfgDescrSGAlt_TypeDef altDesc;

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

  // Configure alternates
  altDesc.peripheral = false;
  altDesc.nMinus1 = TRANSFER_SIZE;

  altDesc.dstInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  altDesc.srcInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  altDesc.size    = DATA_SIZE;
  altDesc.arbRate = ARB_SIZE;
  altDesc.hprot   = 0;

  // Configure alternate descriptors for future use
  for (i = 0; i < LIST_SIZE; i++){
    altDesc.src   = &srcBuffer[BUFFER_SIZE * i];
    altDesc.dst   = &dstBuffer[LIST_SIZE - 1 - i];

    DMA_CfgDescrScatterGather(altDescrs, i, &altDesc);
  }

  // Start DMA
  DMA_ActivateScatterGather(
      DMA_CHANNEL,
      false,
      altDescrs,
      LIST_SIZE);
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
