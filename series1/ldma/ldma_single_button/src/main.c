/**************************************************************************//**
 * @file
 * @brief LDMA Single Button Example
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

/* DMA channel used for the examples */
#define DMA_CHANNEL         0
#define DMA_CH_MASK         1 << DMA_CHANNEL

/* Memory to memory transfer buffer size and constant for GPIO PRS */
#define BUFFER_SIZE         128
#define TRANSFER_SIZE       BUFFER_SIZE - 1
#define GPIO_PRS_CHANNEL    1

/* Buffer for memory to memory transfer */
uint16_t srcBuffer[BUFFER_SIZE];
uint16_t dstBuffer[BUFFER_SIZE];

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
 * @brief Setup push button BTN1 as PRS source for DMAREQ0.
 *****************************************************************************/
static void gpioPrsSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure push button BTN1 as input */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);
  /* Configure interrupt on push button PB1 for rising edge but not enabled - PRS sensing instead */
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, true, false, false);

  /* Select GPIO as PRS source and push button BTN1 as signal for PRS channel */
  CMU_ClockEnable(cmuClock_PRS, true);

  if ( BSP_GPIO_PB1_PIN > 7 ){
    PRS_SourceSignalSet(GPIO_PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOH, (uint32_t)(BSP_GPIO_PB1_PIN - 8), prsEdgePos);
  }
  else {
    PRS_SourceSignalSet(GPIO_PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOL, BSP_GPIO_PB1_PIN, prsEdgePos);
  }

  /* Select PRS channel for DMA request 0 */
  PRS->DMAREQ0 = PRS_DMAREQ0_PRSSEL_PRSCH1;
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for single direct register transfer
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  /* Initialize buffers for memory transfer */
  for (i = 0; i < BUFFER_SIZE; i++){
    srcBuffer[i] = i;
    dstBuffer[i] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );

  /* Writes directly to the LDMA channel registers */
  LDMA->CH[DMA_CHANNEL].CTRL =
      LDMA_CH_CTRL_SIZE_HALFWORD
      | LDMA_CH_CTRL_REQMODE_ALL
      | LDMA_CH_CTRL_BLOCKSIZE_UNIT4
      | (TRANSFER_SIZE) << _LDMA_CH_CTRL_XFERCNT_SHIFT;
  LDMA->CH[DMA_CHANNEL].SRC = (uint32_t)&srcBuffer;
  LDMA->CH[DMA_CHANNEL].DST = (uint32_t)&dstBuffer;

  /* Enable interrupt and wait PRS on DMAREQ0 to start transfer */
  LDMA->CH[DMA_CHANNEL].REQSEL = ldmaPeripheralSignal_PRS_REQ0;
  LDMA->IFC = DMA_CH_MASK;
  LDMA->IEN = DMA_CH_MASK;

  /* Enable LDMA Channel */
  LDMA->CHEN = DMA_CH_MASK;
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

  /* Initialize GPIO for PRS */
  gpioPrsSetup();

  /* Initialize LDMA */
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
