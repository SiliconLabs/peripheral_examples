/***************************************************************************//**
 * @file main.c
 * @brief DMA transfered in a loop until a predetermined number of loops have
 * been completed, then continue through a linked list
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// Transfer buffer size
#define BUFFER_SIZE         4
#define ARB_SIZE            dmaArbitrate4

#define TRANSFER_SIZE       BUFFER_SIZE - 1

// Number of iterations of A and B.
#define NUM_ITERATIONS      8

// Buffer for memory to memory transfer
uint8_t dstBuffer[BUFFER_SIZE];

uint8_t srcA[BUFFER_SIZE] = "AAaa";
uint8_t srcB[BUFFER_SIZE] = "BBbb";
uint8_t srcC[BUFFER_SIZE] = "CCcc";

uint32_t ppCnt;

DMA_CB_TypeDef dmacb;

/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  ppCnt++;

  if (ppCnt > NUM_ITERATIONS){
    return;
  }

  if (ppCnt == NUM_ITERATIONS){
    // Start C
    DMA_ActivateBasic(
        DMA_CHANNEL,
        true,
        false,
        dstBuffer,        // destination
        srcC,             // source
        TRANSFER_SIZE);
  }

  // Request via software
  DMA->CHSWREQ |= DMA_CH_MASK;
}

/***************************************************************************//**
 * @brief
 *   Initialize the DMA controller for descriptor list with looping
 ******************************************************************************/
void initDma(void)
{
  uint32_t i;

  ppCnt = 0;

  /* Initialize buffers for memory transfer */
  for (i = 0; i < BUFFER_SIZE; i++){
    dstBuffer[i] = 0;
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
  chnlCfg.select    = 0;          // software triggered
  chnlCfg.cb        = &dmacb;
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

  descrCfg.dstInc  = dmaDataInc1;
  descrCfg.srcInc  = dmaDataInc1;
  descrCfg.size    = dmaDataSize1;
  descrCfg.arbRate = ARB_SIZE;
  descrCfg.hprot   = 0;

  DMA_CfgDescr(DMA_CHANNEL, true, &descrCfg);   // configure as primary
  DMA_CfgDescr(DMA_CHANNEL, false, &descrCfg);  // configure as alternate

  // Configure loop transfer mode
  DMA_CfgLoop_TypeDef loopConfig;
  loopConfig.enable  = true;                // Enable looping
  loopConfig.nMinus1 = TRANSFER_SIZE;       // Reload value for number of DMA transfers minus 1
  DMA_CfgLoop(DMA_CHANNEL, &loopConfig);

  // Start DMA
  DMA_ActivatePingPong(
      DMA_CHANNEL,
      true,
      dstBuffer,        // primary destination
      srcA,             // primary source
      TRANSFER_SIZE,
      dstBuffer,        // alternate destination
      srcB,             // alternate source
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
    EMU_EnterEM1();
  }
}
