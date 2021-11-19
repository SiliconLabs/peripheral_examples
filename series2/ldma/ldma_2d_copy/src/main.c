/***************************************************************************//**
 * @file main.c
 * @brief This example demonstrates the LDMA 2D copy. See readme.txt for details
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

#include <stdint.h>
#include "em_chip.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_ldma.h"
#include "em_cmu.h"

// DMA channel used for the example
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        (1 << LDMA_CHANNEL)

// 2D buffer size and constants for 2D copy
#define BUFFER_2D_WIDTH     10
#define BUFFER_2D_HEIGHT    8
#define TRANSFER_WIDTH      3
#define TRANSFER_HEIGHT     4
#define SRC_ROW_INDEX       1
#define SRC_COL_INDEX       0
#define DST_ROW_INDEX       1
#define DST_COL_INDEX       2

// Descriptor linked list for LDMA transfer
LDMA_Descriptor_t descLink[2];

// Buffers for 2D copy transfer
uint16_t src2d[BUFFER_2D_HEIGHT][BUFFER_2D_WIDTH];
uint16_t dst2d[BUFFER_2D_HEIGHT][BUFFER_2D_WIDTH];

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
 *   Initialize the LDMA controller for 2D copy
 ******************************************************************************/
void initLdma(void)
{
  uint32_t x, y;

  // Initialize buffers for 2D copy
  for (x = 0; x < BUFFER_2D_HEIGHT; x++)
  {
    for (y = 0; y < BUFFER_2D_WIDTH; y++)
    {
      src2d[x][y] = x*BUFFER_2D_WIDTH + y;
      dst2d[x][y] = 0;
    }
  }

  // Initialize LDMA clock
  CMU_ClockEnable(cmuClock_LDMA, true);
  /* Note: For EFR32xG21 radio devices, library function calls to
   * CMU_ClockEnable() have no effect as oscillators are automatically turned
   * on/off based on demand from the peripherals; CMU_ClockEnable() is a dummy
   * function for EFR32xG21 for library consistency/compatibility.
   */

  // Initialize LDMA
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Use looped memory transfer configuration macro
  LDMA_TransferCfg_t memTransfer = LDMA_TRANSFER_CFG_MEMORY_LOOP(
      TRANSFER_HEIGHT-2);

  // First descriptor gets the absolute source and destination address
  descLink[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_HALF(
      &src2d[SRC_ROW_INDEX][SRC_COL_INDEX],
      &dst2d[DST_ROW_INDEX][DST_COL_INDEX],
      TRANSFER_WIDTH,
      1);

  // Second descriptor uses relative addressing and performs looping
  descLink[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_HALF(
      (BUFFER_2D_WIDTH - TRANSFER_WIDTH)*2,
      (BUFFER_2D_WIDTH - TRANSFER_WIDTH)*2,
      TRANSFER_WIDTH,
      0);

  // Use relative addressing for source & destination, enable looping
  descLink[1].xfer.srcAddrMode = ldmaCtrlSrcAddrModeRel;
  descLink[1].xfer.dstAddrMode = ldmaCtrlDstAddrModeRel;
  descLink[1].xfer.decLoopCnt = 1;

  // Stop after looping
  descLink[1].xfer.link = 0;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&memTransfer, (void*)&descLink);
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
