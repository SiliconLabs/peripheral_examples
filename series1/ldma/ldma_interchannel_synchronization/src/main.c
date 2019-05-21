/**************************************************************************//**
 * @file
 * @brief LDMA Inter-channel Synchronization Example
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
#include "em_gpio.h"
#include "em_ldma.h"
#include "em_prs.h"
#include "bsp.h"


/* Constants for inter-channel sync transfer */
#define SYNC_SET            0x80
#define SYNC_CLEAR          0x80
#define MATCH_VALUE         0x80

/* Channels used for the example */
#define LDMA_CHANNEL        0
#define GPIO_PRS_CHANNEL    1

/* Memory to memory transfer buffer size */
#define BUFFER_SIZE         4

/* Descriptor linked list for LDMA transfer */
LDMA_Descriptor_t descLink0[3];
LDMA_Descriptor_t descLink1[2];

/* Buffer for memory to memory transfer */
uint8_t srcA[BUFFER_SIZE] = "AAaa";
uint8_t srcC[BUFFER_SIZE] = "CCcc";
uint8_t srcY[BUFFER_SIZE] = "YYyy";

uint8_t dstBuffer[BUFFER_SIZE];

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
  if ( pending & LDMA_IF_ERROR ){
    /* Loop here to enable the debugger to see what has happened */
    while (1);
  }
}

/**************************************************************************//**
 * @brief Setup push button BTN0 and BTN1 as PRS source for DMAREQ0 and DMAREQ1.
 *****************************************************************************/
static void gpioPrsSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure push buttons as input */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);

  /* Configure interrupt on push buttons for rising edge but not enabled - PRS sensing instead */
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, true, false, false);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, true, false, false);

  /* Select GPIO as PRS source and push button BTN1 as signal for PRS channel */
  CMU_ClockEnable(cmuClock_PRS, true);

  if ( BSP_GPIO_PB0_PIN > 7 ){
    PRS_SourceSignalSet(GPIO_PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOH, (uint32_t)(BSP_GPIO_PB0_PIN - 8), prsEdgePos);
  }
  else {
    PRS_SourceSignalSet(GPIO_PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOL, BSP_GPIO_PB0_PIN, prsEdgePos);
  }
  if ( BSP_GPIO_PB1_PIN > 7 ){
    PRS_SourceSignalSet(GPIO_PRS_CHANNEL+1, PRS_CH_CTRL_SOURCESEL_GPIOH, (uint32_t)(BSP_GPIO_PB1_PIN - 8), prsEdgePos);
  }
  else {
    PRS_SourceSignalSet(GPIO_PRS_CHANNEL+1, PRS_CH_CTRL_SOURCESEL_GPIOL, BSP_GPIO_PB1_PIN, prsEdgePos);
  }

  /* Select PRS channel for DMA request 0 */
  PRS->DMAREQ0 = PRS_DMAREQ0_PRSSEL_PRSCH1;
  PRS->DMAREQ1 = PRS_DMAREQ0_PRSSEL_PRSCH2;
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for inter-channel synchronization
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  /* Initialize buffers for memory transfer */
  for (i = 0; i < BUFFER_SIZE; i++){
    dstBuffer[i] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );

  /* Use peripheral transfer configuration macro for first DMA channel */
  LDMA_TransferCfg_t periTransferPB0 = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_PRS_REQ0);
  LDMA_TransferCfg_t periTransferPB1 = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_PRS_REQ1);

  /* Set up descriptor links */
  /* Transfer AAaa */
  descLink0[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(&srcA, &dstBuffer, BUFFER_SIZE, 1);
  /* Wait for MATCH_VALUE to be set */
  descLink0[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0, MATCH_VALUE, MATCH_VALUE, 1);
  /* Transfer CCcc */
  descLink0[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2M_BYTE(&srcC, &dstBuffer, BUFFER_SIZE);

  /* Transfer YYyy */
  descLink1[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(&srcY, &dstBuffer, BUFFER_SIZE, 1);
  /* Set SYNC_SET value */
  descLink1[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_SYNC(SYNC_SET, 0, 0, 0);

  /* First transfer in each set waits for request */
  descLink0[0].xfer.structReq = false;
  descLink1[0].xfer.structReq = false;

  /* Start both channels */
  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferPB0, (void*)&descLink0);
  LDMA_StartTransfer(LDMA_CHANNEL+1, (void*)&periTransferPB1, (void*)&descLink1);
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

  /* Initialize GPIO/PRS */
  gpioPrsSetup();
  /* Initialize LDMA */
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
