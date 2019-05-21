/***************************************************************************//**
 * @file
 * @brief This example demonstrates the LDMA scatter-gather transfer.
 *        See readme.txt for details.
 * @version 0.0.1
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2019 Silicon Labs, http://www.silabs.com</b>
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
#include "em_ldma.h"

// DMA channel used for the example
#define LDMA_CHANNEL         0
#define LDMA_CH_MASK         (1 << LDMA_CHANNEL)

// Memory to memory transfer buffer size
#define BUFFER_SIZE          8

// Descriptor linked list size
#define LIST_SIZE            4
#define LAST_INDEX           LIST_SIZE - 1

// Descriptor linked list for LDMA transfer
LDMA_Descriptor_t descLink[LIST_SIZE];

// Buffers for linked and Scatter Gather transfer
uint16_t srcBuffer[LIST_SIZE * BUFFER_SIZE];
uint16_t dstBuffer[LIST_SIZE][BUFFER_SIZE];

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler( void )
{
  uint32_t pending;

  // Read interrupt source
  pending = LDMA_IntGet();

  // Clear interrupts
  LDMA_IntClear(pending);

  // Check for LDMA error
  if ( pending & LDMA_IF_ERROR ){
    // Loop here to enable the debugger to see what has happened
    while (1);
  }
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for scatter transfer.
 *   Scatter data from one larger array into smaller arrays. 
 ******************************************************************************/
void initLdmaScatter(void)
{
  uint32_t i;

  // Fill source buffer and clear destination buffer
  for (i = 0; i < BUFFER_SIZE * LIST_SIZE; i++){
    srcBuffer[i] = i;
    dstBuffer[i / BUFFER_SIZE][i % BUFFER_SIZE] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );

  // Use memory transfer configuration macro
  LDMA_TransferCfg_t periTransferTx = LDMA_TRANSFER_CFG_MEMORY();

  // First Descriptor
  descLink[0] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_M2M_HALF( &srcBuffer, &dstBuffer[LAST_INDEX],
                                     BUFFER_SIZE, 1 );

  // Middle Descriptors
  for(i = 1; i < LIST_SIZE - 1; i++){
    descLink[i] = (LDMA_Descriptor_t)
        LDMA_DESCRIPTOR_LINKREL_M2M_HALF( 0, &dstBuffer[LAST_INDEX - i],
                                         BUFFER_SIZE, 1 );

    // Descriptor source is relative
    descLink[i].xfer.srcAddrMode = ldmaCtrlSrcAddrModeRel;
  }

  // Last Descriptor
  descLink[LIST_SIZE - 1] = (LDMA_Descriptor_t)
      LDMA_DESCRIPTOR_SINGLE_M2M_HALF( 0, &dstBuffer[0], BUFFER_SIZE );

  // Last Descriptor source is relative
  descLink[LIST_SIZE - 1].xfer.srcAddrMode = ldmaCtrlSrcAddrModeRel;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink[0]);
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for gather transfer.
 *   Gather data from smaller arrays into one larger array. 
 ******************************************************************************/
void initLdmaGather(void)
{
  uint32_t i;

  // Clear source buffer and keep destination buffer
  // Destination buffer is source and soruce buffer is destination
  for (i = 0; i < BUFFER_SIZE * LIST_SIZE; i++){
    srcBuffer[i] = 0;
  }

  // Use memory transfer configuration macro
  LDMA_TransferCfg_t memTransferTx = LDMA_TRANSFER_CFG_MEMORY();

  // First Descriptor
  descLink[0] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_M2M_HALF( &dstBuffer[LAST_INDEX], &srcBuffer,
                                     BUFFER_SIZE, 1 );

  // Middle Descriptors
  for(i = 1; i < LIST_SIZE - 1; i++){
    descLink[i] = (LDMA_Descriptor_t)
        LDMA_DESCRIPTOR_LINKREL_M2M_HALF( &dstBuffer[LAST_INDEX - i], 0,
                                         BUFFER_SIZE, 1 );

    // Descriptor destination is relative
    descLink[i].xfer.dstAddrMode = ldmaCtrlDstAddrModeRel;
  }

  // Last Descriptor
  descLink[LIST_SIZE - 1] = (LDMA_Descriptor_t)
      LDMA_DESCRIPTOR_SINGLE_M2M_HALF( &dstBuffer[0], 0, BUFFER_SIZE );

  // Last Descriptor destination is relative
  descLink[LIST_SIZE - 1].xfer.dstAddrMode = ldmaCtrlDstAddrModeRel;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&memTransferTx, (void*)&descLink[0]);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator if available
#if defined( _EMU_DCDCCTRL_MASK )  
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
#endif
  
  // Initialize LDMA for Scatter transfer
  initLdmaScatter();
  EMU_EnterEM1();

  // Initialize LDMA for Gather transfer
  initLdmaGather();

  while (1)
  {
    EMU_EnterEM1();
  }
}
