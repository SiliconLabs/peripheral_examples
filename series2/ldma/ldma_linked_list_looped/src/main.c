/***************************************************************************//**
 * @file main.c
 * @brief This example demonstrates the LDMA descriptor list with looping. See
 * readme.txt for details.
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
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        (1 << LDMA_CHANNEL)

// Memory to memory transfer buffer size
#define BUFFER_SIZE         4

// Descriptor linked list size
#define LIST_SIZE           3

// Number of iterations of A and B.
#define NUM_ITERATIONS      4

// Constant for loop transfer
// NUM_ITERATIONS - 1 (for first iteration)
#define LOOP_COUNT          NUM_ITERATIONS - 1

// Descriptor linked list for LDMA transfer
LDMA_Descriptor_t descLink[LIST_SIZE];

// Buffers for memory to memory transfer
uint8_t dstBuffer[BUFFER_SIZE];

uint8_t srcA[BUFFER_SIZE] = "AAaa";
uint8_t srcB[BUFFER_SIZE] = "BBbb";
uint8_t srcC[BUFFER_SIZE] = "CCcc";

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

  // Request next transfer
  LDMA->SWREQ |= LDMA_CH_MASK;
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for descriptor list with looping
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  // Initialize buffers for memory transfer
  for (i = 0; i < BUFFER_SIZE; i++)
  {
    dstBuffer[i] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Use looped memory transfer configuration macro
  LDMA_TransferCfg_t periTransferTx =
      LDMA_TRANSFER_CFG_MEMORY_LOOP(LOOP_COUNT);

  // LINK descriptor macros for looping
  descLink[0] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(&srcA, &dstBuffer, BUFFER_SIZE, 1);
  descLink[1] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(&srcB, &dstBuffer, BUFFER_SIZE, -1);

  // SINGLE descriptor macro for single transfer
  descLink[2] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_SINGLE_M2M_BYTE(&srcC, &dstBuffer, BUFFER_SIZE);

  // Enable looping
  descLink[1].xfer.decLoopCnt = 1;

  // Enable interrupts
  descLink[0].xfer.doneIfs = true;
  descLink[1].xfer.doneIfs = true;
  descLink[2].xfer.doneIfs = true;

  // Disable automatic triggers
  descLink[0].xfer.structReq = false;
  descLink[1].xfer.structReq = false;
  descLink[2].xfer.structReq = false;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink);

  // Request first transfer
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
