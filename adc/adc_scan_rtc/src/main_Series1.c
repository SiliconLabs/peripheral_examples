/**************************************************************************//**
 * @file
 * @brief Use the ADC to take a nonblocking measurement
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
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
#include "em_rtcc.h"
#include "em_prs.h"
#include "bsp.h"
#include "retargetserial.h"

#define LFRCO_FREQUENCY 32768
#define WAKEUP_INTERVAL_MS 500
#define WAKEUP_COUNT (((LFRCO_FREQUENCY * WAKEUP_INTERVAL_MS) / 1000) - 1)

uint32_t sample;
uint32_t inputMv;
uint32_t vddMv;

void ADC0_IRQHandler(void)
{
  // Read scan data from SCAN FIFO
  inputMv = (ADC0->SCANDATAX & _ADC_SCANDATAX_DATA_MASK) * 3300 / 4096;
  vddMv = (ADC0->SCANDATAX & _ADC_SCANDATAX_DATA_MASK) * 3300 / 4096;
}

void initRtcc(void)
{
  // Start LFRCO and wait until it is stable
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

  // Enable clock to the interface of the low energy modules
  CMU_ClockEnable(cmuClock_CORELE, true);
  // Route the LFRCO clock to the RTCC
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_RTCC, true);

  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  RTCC_CCChConf_TypeDef rtccInitCompareChannel = RTCC_CH_INIT_COMPARE_DEFAULT;

  rtccInit.cntWrapOnCCV1 = true; // Clear counter on compare match
  rtccInit.presc = rtccCntPresc_1;

  // Set the compare value of the RTCC
  RTCC_ChannelInit(1, &rtccInitCompareChannel);
  RTCC_ChannelCCVSet(1, WAKEUP_COUNT);

  // Initialize the RTCC
  RTCC_Init(&rtccInit);

  // Use RTCC as async PRS to trigger ADC in EM2
  CMU_ClockEnable(cmuClock_PRS, true);
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_RTCC,
                           PRS_CH_CTRL_SIGSEL_RTCCCCV1);
}

void initAdc(void)
{
  // Enable ADC clock
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef initScan = ADC_INITSCAN_DEFAULT;

  // Select AUXHFRCO for ADC ASYNC mode so it can run in EM2
  CMU->ADCCTRL = CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO;

  // Set AUXHFRCO frequency and use it to setup the ADC
  CMU_AUXHFRCOFreqSet(cmuAUXHFRCOFreq_4M0Hz);
  init.timebase = ADC_TimebaseCalc(CMU_AUXHFRCOBandGet());
  init.prescale = ADC_PrescaleCalc(16000000, CMU_AUXHFRCOBandGet());
  init.em2ClockConfig = adcEm2ClockOnDemand; // Let the ADC enable its clock on demand in EM2

  // Add external ADC input to scan. See README for corresponding EXP header pin.
  ADC_ScanSingleEndedInputAdd(&initScan, adcScanInputGroup0, adcPosSelAPORT1YCH9);

  // Add VDD to scan for demonstration purposes
  ADC_ScanSingleEndedInputAdd(&initScan, adcScanInputGroup1, adcPosSelAVDD);

  // Basic ADC scan configuration
  initScan.diff = 0; // single-ended
  initScan.reference  = adcRefVDD; // VDD reference
  initScan.resolution = adcRes12Bit; // 12-bit resolution

  // Enable PRS trigger and select channel 0
  initScan.prsEnable = true;
  initScan.prsSel = adcPRSSELCh0;

  initScan.fifoOverwrite = true;

  ADC_InitScan(ADC0, &initScan);

  // Set scan data valid level (DVL) to 2
  // This means the SCAN interrupt will be triggered when the ADC SCAN FIFO
  // fills with 2 elements
  ADC0->SCANCTRLX |= (2 - 1) << _ADC_SCANCTRLX_DVL_SHIFT;

  // Enable SCAN interrupt
  ADC_IntEnable(ADC0, ADC_IEN_SCAN);

  // Initialize ADC
  ADC_Init(ADC0, &init);

  // Clear the SCAN FIFO and pending interrupts
  ADC0->SCANFIFOCLEAR = ADC_SCANFIFOCLEAR_SCANFIFOCLEAR;
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  // Enable interrupts
  NVIC_EnableIRQ(ADC0_IRQn);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  initAdc();

  // Set up RTCC to trigger ADC via PRS in 500ms intervals
  initRtcc();

  // Initialize USART, enable VCOM, and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  // Infinite loop
  while(1)
  {
    // Enter EM2 until next ADC interrupt
    EMU_EnterEM2(false);

    printf("Input: %dmV, VDD: %dmV\n", (unsigned int) inputMv, (unsigned int) vddMv);

    // Wait for print to finish by polling the TX Complete (TXC) bit
    while(!(RETARGET_UART->STATUS & USART_STATUS_TXC));
  }

}
