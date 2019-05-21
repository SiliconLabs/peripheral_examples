/***************************************************************************//**
 * @file
 * @brief This example demonstrates the single direct register LDMA transfer.
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

#include "em_chip.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "em_prs.h"
#include "bsp.h"

// DMA channel used for the example
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        (1 << LDMA_CHANNEL)

// Memory to memory transfer buffer size and constants for GPIO PRS
#define BUFFER_SIZE         128
#define TRANSFER_SIZE       BUFFER_SIZE - 1
#define USE_GPIO_PRS        1    
#define GPIO_PRS_CHANNEL    1

// Buffers for memory to memory transfer
uint16_t srcBuffer[BUFFER_SIZE];
uint16_t dstBuffer[BUFFER_SIZE];

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

#if (USE_GPIO_PRS == 1)
/***************************************************************************//**
 * @brief
 *   Setup push button PB1 as PRS source for DMAREQ0.
 ******************************************************************************/
static void gpioPrsSetup(void)
{
  // Configure push button PB1 as input
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN,
                  gpioModeInputPullFilter, 1);

  // Configure PRS sensing on push button PB1
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, false, false);

  // Select GPIO as PRS source and push button PB1 as signal for PRS channel
  PRS_SourceAsyncSignalSet(GPIO_PRS_CHANNEL, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           PRS_ASYNC_CH_CTRL_SIGSEL_GPIOPIN3);

  // DMA request is high active, invert PRS signal to default low
  PRS_Combine(GPIO_PRS_CHANNEL, GPIO_PRS_CHANNEL, prsLogic_NOT_A);

  // Select PRS channel for DMA request 0
  PRS_ConnectConsumer(GPIO_PRS_CHANNEL, prsTypeAsync, prsConsumerLDMA_REQUEST0);
}
#endif

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for single direct register transfer
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  // Initialize buffers for memory transfer
  for (i = 0; i < BUFFER_SIZE; i++){
    srcBuffer[i] = i;
    dstBuffer[i] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );
  
  // Writes directly to the LDMA channel registers
  LDMA->CH[LDMA_CHANNEL].CTRL =
      LDMA_CH_CTRL_SIZE_HALFWORD
      | LDMA_CH_CTRL_REQMODE_ALL
      | LDMA_CH_CTRL_BLOCKSIZE_UNIT4
      | (TRANSFER_SIZE) << _LDMA_CH_CTRL_XFERCNT_SHIFT;
  LDMA->CH[LDMA_CHANNEL].SRC = (uint32_t)&srcBuffer;
  LDMA->CH[LDMA_CHANNEL].DST = (uint32_t)&dstBuffer;

#if (USE_GPIO_PRS == 1)
  // Wait PRS on DMAREQ0 to start transfer
  LDMAXBAR->CH[LDMA_CHANNEL].REQSEL = LDMAXBAR_CH_REQSEL_SIGSEL_LDMAXBARPRSREQ0
    | LDMAXBAR_CH_REQSEL_SOURCESEL_LDMAXBAR;
#else
  // Software to start transfer
  LDMAXBAR->CH[LDMA_CHANNEL].REQSEL = ldmaPeripheralSignal_NONE;
#endif

  // Enable interrupt 
  LDMA->IF_CLR = LDMA_CH_MASK;
  LDMA->IEN = LDMA_CH_MASK;

  // Enable LDMA Channel
  LDMA->CHEN = LDMA_CH_MASK;

#if (USE_GPIO_PRS == 0)
  // Request transfer
  LDMA->SWREQ |= LDMA_CH_MASK;
#endif
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
  
#if (USE_GPIO_PRS == 1)
  // Initialize GPIO for PRS
  gpioPrsSetup();
#endif
  
  // Initialize LDMA
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
