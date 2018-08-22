/**************************************************************************//**
 * @file
 * @brief LDMA Linked List Example
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
#include "em_ldma.h"

/* DMA channel used for the examples */
#define LDMA_CHANNEL      0
#define LDMA_CH_MASK      1 << LDMA_CHANNEL

/* Memory to memory transfer buffer size and constant for GPIO PRS */
#define BUFFER_SIZE       32

/* Descriptor linked list size */
#define LIST_SIZE         4

/* Descriptor linked list for LDMA transfer */
LDMA_Descriptor_t descLink[LIST_SIZE];

/* Buffer for linked and looped transfer */
uint16_t srcBuffer[LIST_SIZE][BUFFER_SIZE];
uint16_t dstBuffer[LIST_SIZE][BUFFER_SIZE];

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler( void )
{
  uint32_t pending;

  /* Read interrupt source */
  pending = LDMA_IntGet();

  /* Clear interrupts */
  LDMA_IntClear(pending);

  /* Check for LDMA error */
  if ( pending & LDMA_IF_ERROR )
  {
    /* Loop here to enable the debugger to see what has happened */
    while (1);
  }

  /* Request next transfer */
  LDMA->SWREQ |= LDMA_CH_MASK;
}

/***************************************************************************//**
 * @brief
 *   Descriptor linked list example.
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  /* Fill buffers */
  for (i = 0; i < BUFFER_SIZE * 4; i++){
    srcBuffer[i / BUFFER_SIZE][i % BUFFER_SIZE] = i;
    dstBuffer[i / BUFFER_SIZE][i % BUFFER_SIZE] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );

  /* Use peripheral transfer configuration macro */
  LDMA_TransferCfg_t periTransferTx = LDMA_TRANSFER_CFG_MEMORY();

  /* LINK descriptor macros to form linked list, SINGLE descriptor macro for the last one */
  for(i = 0; i < LIST_SIZE - 1; i++)
  {
    descLink[i] = (LDMA_Descriptor_t)
        LDMA_DESCRIPTOR_LINKREL_M2M_HALF( &srcBuffer[i], &dstBuffer[i], BUFFER_SIZE, 1 );
  }

  descLink[LIST_SIZE - 1] = (LDMA_Descriptor_t)
      LDMA_DESCRIPTOR_SINGLE_M2M_HALF( &srcBuffer[LIST_SIZE - 1], &dstBuffer[LIST_SIZE - 1], BUFFER_SIZE );

  /* Turn on Done interrupts for Descriptor 2 */
  descLink[1].xfer.doneIfs = true;
  /* Disable automatic triggers for Descriptor 3 */
  descLink[2].xfer.structReq = false;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink[0]);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator if available */
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  /* Initialize LDMA */
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
