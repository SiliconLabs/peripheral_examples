/**************************************************************************//**
 * @file
 * @brief Use the ADC to take nonblocking measurements in EM1.  The PRS routes
 * gpio signals to trigger ADC Scan conversions.  The DMA moves the conversion
 * results to a software buffer.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_adc.h"
#include "em_prs.h"
#include "em_dma.h"
#include "bsp.h"

#include "dmactrl.h"

// Change this to increase or decrease number of samples.
#define ADC_BUFFER_SIZE   16

#define ADC_DVL           2
#define ADC_FREQ          13000000

#define DMA_CHANNEL       0
#define PRS_CHANNEL       0

// Buffer for ADC single and scan conversion
uint32_t adcBuffer[ADC_BUFFER_SIZE];

DMA_CB_TypeDef dmacb;

/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  // Insert transfer complete functionality here
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable clock for GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set Push Button 0 to input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);

  // Configure Push Button 0 to create PRS interrupt signals only
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, false, false);

  // Enable and configure PRS
  CMU_ClockEnable(cmuClock_PRS, true);
  if (BSP_GPIO_PB0_PIN > 7)
  {
    PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, (uint32_t)(BSP_GPIO_PB0_PIN - 8), prsEdgePos);
  }
  else
  {
    PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOL, BSP_GPIO_PB0_PIN, prsEdgePos);
  }
}

/**************************************************************************//**
 * @brief DMA initialization
 *****************************************************************************/
void initDma(void)
{
  // Enable CMU clock
  CMU_ClockEnable(cmuClock_DMA, true);

  // Basic DMA configuration
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;

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
  chnlCfg.select    = DMAREQ_ADC0_SCAN;
  chnlCfg.cb        = &dmacb;
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

  descrCfg.dstInc  = dmaDataInc4;
  descrCfg.srcInc  = dmaDataIncNone;
  descrCfg.size    = dmaDataSize4;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(DMA_CHANNEL, true, &descrCfg);

  // Configure loop transfer mode
  DMA_CfgLoop_TypeDef loopConfig;
  loopConfig.enable  = true;                // Enable looping
  loopConfig.nMinus1 = ADC_BUFFER_SIZE - 1; // Reload value for number of DMA transfers minus 1
  DMA_CfgLoop(DMA_CHANNEL, &loopConfig);

  // Start DMA
  DMA_ActivateBasic(DMA_CHANNEL,  // channel
      true,                       // Is primary
      false,                      // Don't use bursts
      adcBuffer,                  // destination
      (void *)&(ADC0->SCANDATA),  // source
      ADC_BUFFER_SIZE - 1);       // transfer size
}

/**************************************************************************//**
 * @brief ADC initialization
 *****************************************************************************/
void initAdc(void)
{
  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef initScan = ADC_INITSCAN_DEFAULT;

  // Enable ADC clock
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);

  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(ADC_FREQ, 0);

  // Add external ADC input to scan. See README for corresponding EXP header pin.
  initScan.input = ADC_SCANCTRL_INPUTMASK_CH6 |
      ADC_SCANCTRL_INPUTMASK_CH7;

  // Basic ADC scan configuration
  initScan.diff       = false;        // scan-ended
  initScan.reference  = adcRef2V5;    // 2.5V reference
  initScan.resolution = adcRes12Bit;  // 12-bit resolution

  // Enable PRS trigger and select channel 0
  initScan.prsEnable = true;
  initScan.prsSel = adcPRSSELCh0;

  // Initialize ADC
  ADC_Init(ADC0, &init);
  ADC_InitScan(ADC0, &initScan);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Set up GPIO to trigger ADC via PRS
  initGpio();
  // Setup ADC to perform conversions
  initAdc();
  // Setup DMA to move ADC results to user memory
  initDma();

  // Infinite loop
  while(1)
  {
    // Enter EM1 until next ADC interrupt
    EMU_EnterEM1();
  }
}
