/**************************************************************************//**
 * @file
 * @brief DMA Ping-Pong transfer between two descriptors
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
#define PP_BUFFER_SIZE      8
#define ARB_SIZE            dmaArbitrate8

#define TRANSFER_SIZE       PP_BUFFER_SIZE - 1

// This should agree with the datatype of the data transfered
#define DATA_SIZE           dmaDataSize2

// Buffer for Ping-Pong transfer
uint16_t srcBuffer[PP_BUFFER_SIZE];
uint16_t dstBuffer[2][PP_BUFFER_SIZE];;

DMA_CB_TypeDef dmacb;

/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  uint32_t i;

  // Increment source buffer to make transfers visible
  for (i = 0; i < PP_BUFFER_SIZE; i++){
    srcBuffer[i]++;
  }

  DMA_RefreshPingPong(
      channel,
      primary,
      false,
      dstBuffer[primary],   // destination
      srcBuffer,            // source
      TRANSFER_SIZE,
      false);               // Do not stop ping-pong

  // Request next transfer via software
  DMA->CHSWREQ |= DMA_CH_MASK;
}

/***************************************************************************//**
 * @brief
 *   Descriptor ping pong example.
 ******************************************************************************/
void initDma(void)
{
  uint32_t i;

  // Initialize buffers for memory transfer
  for (i = 0; i < PP_BUFFER_SIZE; i++){
    srcBuffer[i] = 1;
    dstBuffer[0][i] = 0;
    dstBuffer[1][i] = 0;
  }

  // Enable DMA clock
  CMU_ClockEnable(cmuClock_DMA, true);

  // Basic DMA configuration
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;

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
  chnlCfg.select    = 0;                // software triggered
  chnlCfg.cb        = &dmacb;
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

  descrCfg.dstInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.srcInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.size    = DATA_SIZE;
  descrCfg.arbRate = ARB_SIZE;
  descrCfg.hprot   = 0;

  DMA_CfgDescr(DMA_CHANNEL, isPrimary, &descrCfg);      // configure as primary
  DMA_CfgDescr(DMA_CHANNEL, !isPrimary, &descrCfg);     // configure as alternate

  // Start DMA
  DMA_ActivatePingPong(
      DMA_CHANNEL,
      true,
      dstBuffer[isPrimary],   // primary destination
      srcBuffer,              // primary source
      TRANSFER_SIZE,
      dstBuffer[!isPrimary],  // alternate destination
      srcBuffer,              // alternate source
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
