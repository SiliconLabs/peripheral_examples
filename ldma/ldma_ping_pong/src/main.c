/**************************************************************************//**
 * @file
 * @brief LDMA Ping-Pong Example
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
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        1 << LDMA_CHANNEL

/* Ping-Pong buffer size and constant for Ping-Pong transfer */
#define PP_BUFFER_SIZE      8

/* Descriptor linked list for LDMA transfer */
LDMA_Descriptor_t descLink[2];

/* Buffer for Ping-Pong transfer */
uint16_t srcBuffer[PP_BUFFER_SIZE];

uint16_t pingBuffer[PP_BUFFER_SIZE];
uint16_t pongBuffer[PP_BUFFER_SIZE];

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler( void )
{
  uint32_t pending, i;

  /* Read interrupt source */
  pending = LDMA_IntGet();

  /* Clear interrupts */
  LDMA_IntClear(pending);

  /* Check for LDMA error */
  if ( pending & LDMA_IF_ERROR ){
    /* Loop here to enable the debugger to see what has happened */
    while (1);
  }

  /* Increment source buffer */
  for (i = 0; i < PP_BUFFER_SIZE; i++){
    srcBuffer[i]++;
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

  /* Initialize Ping-Pong buffers */
  for (i = 0; i < PP_BUFFER_SIZE; i++){
    srcBuffer[i] = 1;
    pingBuffer[i] = 0;
    pongBuffer[i] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );

  /* Use peripheral transfer configuration macro */
  LDMA_TransferCfg_t periTransferTx = LDMA_TRANSFER_CFG_MEMORY();

  /* LINK descriptor macros for Ping-Pong transfer */
  descLink[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_HALF(&srcBuffer, &pingBuffer, PP_BUFFER_SIZE, 1);
  descLink[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_HALF(&srcBuffer, &pongBuffer, PP_BUFFER_SIZE, -1);

  /* Enable interrupts */
  descLink[0].xfer.doneIfs = true;
  descLink[1].xfer.doneIfs = true;

  /* Disable automatic transfers */
  descLink[0].xfer.structReq = false;
  descLink[1].xfer.structReq = false;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink);

  /* Software request to start transfer */
  LDMA->SWREQ |= LDMA_CH_MASK;
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
