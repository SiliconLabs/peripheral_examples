/***************************************************************************//**
 * @file main.c
 * @brief This example demonstrates the LDMA single descriptor looped transfer.
 * See readme.txt for details.
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

#include "em_chip.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_ldma.h"

// DMA channel used for the example
#define LDMA_CHANNEL      0
#define LDMA_CH_MASK      (1 << LDMA_CHANNEL)

// Memory to memory transfer buffer size
#define BUFFER_SIZE       4

// Number of sets of BUFFER_SIZE elements to send.
// Total words sent = BUFFER_SIZE * NUM_ITERATIONS
#define NUM_ITERATIONS    4

// Constant for loop transfer
// NUM_ITERATIONS - 1 (for first descriptor) - 1 (for first iteration)
#define LOOP_COUNT        NUM_ITERATIONS - 1

// Descriptor linked list for LDMA transfer
LDMA_Descriptor_t descLink;

// Buffers for memory to memory transfer
uint32_t srcBuffer[BUFFER_SIZE];
uint32_t dstBuffer[NUM_ITERATIONS][BUFFER_SIZE];

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler(void)
{
  uint32_t pending;

  // Read interrupt source
  pending = LDMA_IntGet();

  // Clear interrupts
  LDMA_IntClear(pending);

  // Check for LDMA error
  if (pending & LDMA_IF_ERROR)
  {
    // Loop here to enable the debugger to see what has happened
    while (1);
  }

  // Start next Transfer
  LDMA->SWREQ |= LDMA_CH_MASK;
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for single descriptor looped transfer
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i, j;

  // Initialize buffers for memory transfer
  for (i = 0; i < BUFFER_SIZE; i++)
  {
    srcBuffer[i] = i;
    for (j = 0; j < NUM_ITERATIONS; j++)
    {
      dstBuffer[j][i] = 0;
    }
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Use looped memory transfer configuration macro
  LDMA_TransferCfg_t periTransferTx = LDMA_TRANSFER_CFG_MEMORY_LOOP(LOOP_COUNT);

  // Use LINK descriptor macro for initialization and looping
  descLink = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_M2M_WORD(&srcBuffer, 0, BUFFER_SIZE, 0);

  descLink.xfer.blockSize   = ldmaCtrlBlockSizeUnit4;   // Set block sizes to 4
  descLink.xfer.doneIfs     = true;                     // Enable interrupts
  descLink.xfer.structReq   = false;                    // Disable auto-requests
  descLink.xfer.decLoopCnt  = 1;                        // Enable loops
  descLink.xfer.link        = 0;                        // End of linked list
  // Each consecutive transfer uses the previous destination
  descLink.xfer.dstAddrMode = ldmaCtrlDstAddrModeRel;
  // Set request mode to Block instead of all
  descLink.xfer.reqMode     = ldmaCtrlReqModeBlock;

  // Start transfers at dstBuffer
  LDMA->CH[LDMA_CHANNEL].DST = (uint32_t)&dstBuffer;

  // Start Transfer
  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink);

  // Send software request
  LDMA->SWREQ |= LDMA_CH_MASK;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator if available
#if defined( DCDC )  
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
#endif
  
  // Initialize LDMA
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
