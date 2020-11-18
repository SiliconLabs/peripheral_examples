/***************************************************************************//**
 * @file main_s1.c
 * @brief Use the ADC to take nonblocking measurements in EM2. The PRS routes
 * gpio signals to trigger ADC Scan conversions. The LDMA moves the conversion
 * results to a software buffer.
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

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_adc.h"
#include "em_prs.h"
#include "em_ldma.h"
#include "bsp.h"

// Change this to increase or decrease number of samples.
#define ADC_BUFFER_SIZE   8

// Change this to set how many samples get sent at once
#define ADC_DVL         2

// Init to max ADC clock for Series 1 with AUXHFRCO
#define ADC_FREQ        4000000

#define LDMA_CHANNEL      0
#define PRS_CHANNEL       0

// Buffer for ADC single and scan conversion
uint32_t adcBuffer[ADC_BUFFER_SIZE];

LDMA_TransferCfg_t trans;
LDMA_Descriptor_t descr;

/**************************************************************************//**
 * @brief LDMA handler
 *****************************************************************************/
void LDMA_IRQHandler(void)
{
  // Clear interrupt flag
  LDMA_IntClear((1 << LDMA_CHANNEL) << _LDMA_IFC_DONE_SHIFT);
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

  // Use GPIO PB0 as async PRS to trigger ADC in EM2
  CMU_ClockEnable(cmuClock_PRS, true);

  if (BSP_GPIO_PB0_PIN > 7){
    PRS_SourceAsyncSignalSet(PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOH, (uint32_t)((uint32_t)(BSP_GPIO_PB0_PIN - 8)));
   } else {
    PRS_SourceAsyncSignalSet(PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOL, BSP_GPIO_PB0_PIN);
  }
}

/**************************************************************************//**
 * @brief LDMA initialization
 *****************************************************************************/
void initLdma(void)
{
  // Enable CMU clock
  CMU_ClockEnable(cmuClock_LDMA, true);

  // Basic LDMA configuration
  LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;

  LDMA_Init(&ldmaInit);

  // Transfer triggers off of ADC Scan completions
  trans = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_ADC0_SCAN);

  descr = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_WORD(
      &(ADC0->SCANDATA),  // source
      adcBuffer,          // destination
      ADC_BUFFER_SIZE,    // data transfer size
      0);                 // link relative offset (links to self)

  descr.xfer.blockSize =ADC_DVL-1;    // transfers ADC_DVL number of units per arbitration cycle
  descr.xfer.ignoreSrec = true; // ignore single requests to decrease energy usage

  // Initialize LDMA transfers
  LDMA_StartTransfer(LDMA_CHANNEL, &trans, &descr);

  // Clear pending and enable interrupts for channel
  NVIC_ClearPendingIRQ(LDMA_IRQn);
  NVIC_EnableIRQ(LDMA_IRQn);
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

  // Select AUXHFRCO for ADC ASYNC mode so it can run in EM2
  CMU->ADCCTRL = CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO;

  // Set AUXHFRCO frequency and use it to setup the ADC
  CMU_AUXHFRCOFreqSet(cmuAUXHFRCOFreq_4M0Hz);
  init.timebase = ADC_TimebaseCalc(CMU_AUXHFRCOBandGet());
  init.prescale = ADC_PrescaleCalc(ADC_FREQ, CMU_AUXHFRCOBandGet());

  // Let the ADC enable its clock on demand in EM2
  init.em2ClockConfig = adcEm2ClockOnDemand;
  // DMA is available in EM2 for processing SCANFIFO DVL request
  initScan.scanDmaEm2Wu = true;

  // Add external ADC input to scan. See README for corresponding EXP header pin.
  // *Note that internal channels are unavailable in ADC scan mode
  ADC_ScanSingleEndedInputAdd(&initScan, adcScanInputGroup0, adcPosSelAPORT2XCH9);
  ADC_ScanSingleEndedInputAdd(&initScan, adcScanInputGroup1, adcPosSelAPORT2YCH10);

  // Basic ADC scan configuration
  initScan.diff       = false;        // single-ended
  initScan.reference  = adcRef2V5;    // 2.5V reference
  initScan.resolution = adcRes12Bit;  // 12-bit resolution
  initScan.acqTime    = adcAcqTime4;  // set acquisition time to meet minimum requirements

  // Enable PRS trigger and select channel 0
  initScan.prsEnable = true;
  initScan.prsSel = (ADC_PRSSEL_TypeDef) PRS_CHANNEL;

  // Initialize ADC
  ADC_Init(ADC0, &init);
  ADC_InitScan(ADC0, &initScan);

  // Set scan data valid level (DVL)
  ADC0->SCANCTRLX = (ADC0->SCANCTRLX & ~_ADC_SCANCTRLX_DVL_MASK) | (((ADC_DVL - 1) << _ADC_SCANCTRLX_DVL_SHIFT) & _ADC_SCANCTRLX_DVL_MASK);

  // Clear the SCAN FIFO
  ADC0->SCANFIFOCLEAR = ADC_SCANFIFOCLEAR_SCANFIFOCLEAR;
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
  initLdma();

  // Infinite loop
  while(1)
  {
    // Enter EM2 until next ADC interrupt
    EMU_EnterEM2(false);
  }
}
