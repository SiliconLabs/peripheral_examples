/**************************************************************************//**
 * @file
 * @brief DMA transfers a submatrix from one block of memory to another
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
#define DMA_CHANNEL       0
#define DMA_CH_MASK       1 << DMA_CHANNEL

// Matrix dimensions
#define BUFFER_2D_WIDTH   10
#define BUFFER_2D_HEIGHT  8
#define TRANSFER_WIDTH    3
#define TRANSFER_HEIGHT   4

// Submatrix location
#define SRC_COL_INDEX     0
#define SRC_ROW_INDEX     1
#define DST_COL_INDEX     1
#define DST_ROW_INDEX     2

#define ARB_SIZE          dmaArbitrate64

// This needs to agree with data size of src2d and dst2d
#define DATA_SIZE         dmaDataSize2

// Buffer for 2D copy transfer
uint16_t src2d[BUFFER_2D_HEIGHT][BUFFER_2D_WIDTH];
uint16_t dst2d[BUFFER_2D_HEIGHT][BUFFER_2D_WIDTH];

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
 *   Initialize the DMA controller for descriptor list with looping
 ******************************************************************************/
void initDma(void)
{
  uint32_t x, y;

  // Initialize buffers for 2D copy
  for(x = 0; x < BUFFER_2D_HEIGHT; x++){
    for(y = 0; y < BUFFER_2D_WIDTH; y++){
      src2d[x][y] = x * BUFFER_2D_WIDTH + y;
      dst2d[x][y] = 0;
    }
  }

  // Enable DMA clock
  CMU_ClockEnable(cmuClock_DMA, true);

  // Basic DMA configuration
  DMA_Init_TypeDef        dmaInit;
  DMA_CfgDescr_TypeDef    descrCfg;
  DMA_CfgChannel_TypeDef  chnlCfg;
  DMA_CfgLoop_TypeDef     loopConfig;
  DMA_CfgRect_TypeDef     rectCfg;

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

  // Configure looping
  loopConfig.nMinus1 = TRANSFER_WIDTH - 1;
  DMA_CfgLoop(DMA_CHANNEL, &loopConfig);

  // Configure Rectangle functionality
  rectCfg.height    = TRANSFER_HEIGHT - 1;
  rectCfg.dstStride = BUFFER_2D_WIDTH << DATA_SIZE;
  rectCfg.srcStride = BUFFER_2D_WIDTH << DATA_SIZE;
  DMA_CfgRect(DMA_CHANNEL, &rectCfg);

  descrCfg.dstInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.srcInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.size    = DATA_SIZE;
  descrCfg.arbRate = ARB_SIZE;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(DMA_CHANNEL, isPrimary, &descrCfg);

  // Start DMA
  DMA_ActivateBasic(
      DMA_CHANNEL,
      isPrimary,
      false,
      &dst2d[DST_ROW_INDEX][DST_COL_INDEX],   // primary destination
      &src2d[SRC_ROW_INDEX][SRC_COL_INDEX],   // primary source
      TRANSFER_WIDTH - 1);

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
