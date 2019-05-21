/**************************************************************************//**
 * @file
 * @brief Single DMA transfer requested by button press
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
#include "em_dma.h"
#include "em_prs.h"
#include "em_timer.h"
#include "bsp.h"

#include "dmactrl.h"

// DMA channel used
#define DMA_CHANNEL         0
#define DMA_CH_MASK         (1 << DMA_CHANNEL)

#define GPIO_PRS_CHANNEL    0

// Transfer buffer size
#define BUFFER_SIZE         16
#define ARB_SIZE            dmaArbitrate16

#define TRANSFER_SIZE       BUFFER_SIZE - 1

// This needs to agree with the datatype of the transferred data
#define DATA_SIZE           dmaDataSize2

// Buffer for memory to memory transfer
uint16_t srcBuffer[BUFFER_SIZE];
uint16_t dstBuffer[BUFFER_SIZE];

DMA_CB_TypeDef dmacb;

/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  // Insert transfer complete functionality here
}

/**************************************************************************//**
 * @brief Setup push button BTN1 as PRS source for TIMER0CC0.
 *****************************************************************************/
void initPrs()
{
  // Enable PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Select PB1 as PRS input signal
  if (BSP_GPIO_PB1_PIN > 7) {
    PRS_SourceSignalSet(GPIO_PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOH,
        (uint32_t)(BSP_GPIO_PB1_PIN - 8), prsEdgePos);
  } else {
    PRS_SourceSignalSet(GPIO_PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOL,
        BSP_GPIO_PB1_PIN, prsEdgePos);
  }
}

/**************************************************************************//**
 * @brief Setup push button BTN1
 *****************************************************************************/
static void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure push button BTN1 as input
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);

  // Configure interrupt on push button PB1 for rising edge but not enabled - PRS sensing instead
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, true, false, false);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure TIMER0 Compare/Capture settings
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  // Set Interrupt flag on every edge
  timerCCInit.eventCtrl = timerEventEveryEdge;
  timerCCInit.edge = timerEdgeBoth;
  timerCCInit.mode = timerCCModeCapture;

  timerCCInit.prsSel = timerPRSSELCh0;
  timerCCInit.prsInput = true;

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Initialize timer with defined prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1;
  TIMER_Init(TIMER0, &timerInit);
}

/***************************************************************************//**
 * @brief
 *   Initialize the DMA controller for single direct register transfer
 ******************************************************************************/
void initDma(void)
{
  uint32_t i;

  // Initialize buffers for memory transfer
  for (i = 0; i < BUFFER_SIZE; i++){
    srcBuffer[i] = i;
    dstBuffer[i] = 0;
  }

  // Enable CMU clock
  CMU_ClockEnable(cmuClock_DMA, true);

  // Basic DMA configuration
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;

  bool isPrimary = true;

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
  chnlCfg.select    = DMAREQ_TIMER0_CC0;  // compare/capture 0
  chnlCfg.cb        = &dmacb;
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

  descrCfg.dstInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.srcInc  = (DMA_DataInc_TypeDef) DATA_SIZE;
  descrCfg.size    = DATA_SIZE;
  descrCfg.arbRate = ARB_SIZE;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(DMA_CHANNEL, isPrimary, &descrCfg);

  // Start DMA
  DMA_ActivateBasic(
      DMA_CHANNEL,      // Channel
      isPrimary,
      false,
      dstBuffer,        // Destination
      srcBuffer,        // Source
      TRANSFER_SIZE);   // Transfer size
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialize peripherals
  initGpio();
  initPrs();
  initTimer();
  initDma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
