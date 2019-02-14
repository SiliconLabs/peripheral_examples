/**************************************************************************//**
 * @file
 * @brief Use the ADC to take repeated nonblocking differential measurements
 * on multiple inputs
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
#include "em_adc.h"

// Init to max ADC clock for Series 1
#define adcFreq   16000000

#define NUM_INPUTS  2

int32_t inputs[NUM_INPUTS];

/**************************************************************************//**
 * @brief  ADC Initializer
 *****************************************************************************/
void initADC (void)
{
  // Enable ADC0 clock
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef initScan = ADC_INITSCAN_DEFAULT;

  // Modify init structs
  init.prescale   = ADC_PrescaleCalc(adcFreq, 0);
  init.timebase = ADC_TimebaseCalc(0);

  initScan.diff       = true;         // differential ended
  initScan.reference  = adcRef2V5;    // internal 2.5V reference
  initScan.resolution = adcRes12Bit;  // 12-bit resolution
  initScan.acqTime    = adcAcqTime4;  // set acquisition time to meet minimum requirements
  initScan.fifoOverwrite = true;      // FIFO overflow overwrites old data

  // Select ADC input. See README for corresponding EXP header pin.
  // Add VDD to scan for demonstration purposes
  ADC_ScanDifferentialInputAdd(&initScan, adcScanInputGroup0, adcPosSelAPORT2XCH9, adcScanNegInput10);
  ADC_ScanDifferentialInputAdd(&initScan, adcScanInputGroup1, adcPosSelAVDD, adcScanNegInputDefault);

  // Set scan data valid level (DVL) to 2
  ADC0->SCANCTRLX |= (NUM_INPUTS - 1) << _ADC_SCANCTRLX_DVL_SHIFT;

  // Clear ADC scan fifo
  ADC0->SCANFIFOCLEAR = ADC_SCANFIFOCLEAR_SCANFIFOCLEAR;

  // Initialize ADC and Scan
  ADC_Init(ADC0, &init);
  ADC_InitScan(ADC0, &initScan);

  // Enable Scan interrupts
  ADC_IntEnable(ADC0, ADC_IEN_SCAN);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
  int16_t data;
  uint32_t i, id;

  // Get ADC results
  for(i = 0; i < NUM_INPUTS; i++)
  {
    // Get ADC scan data
    data = (int16_t)ADC_DataIdScanGet(ADC0, &id);

    // Convert to mV and store in array
    inputs[i] = data * 2500 / 4096;
  }

  // Start next ADC conversion
  ADC_Start(ADC0, adcStartScan);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  initADC();

  ADC_Start(ADC0, adcStartScan); // start first conversion

  // Infinite loop
  while(1);
}
