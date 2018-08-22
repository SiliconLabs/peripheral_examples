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
#include "em_rtc.h"
#include "bsp.h"
#include "retargetserial.h"

#define LFRCO_FREQUENCY 32768
#define WAKEUP_INTERVAL_MS 500
#define WAKEUP_COUNT (((LFRCO_FREQUENCY * WAKEUP_INTERVAL_MS) / 1000) - 1)

uint32_t sample;
uint32_t ch7Mv;
uint32_t ch0Mv;

/**************************************************************************//**
 * @brief RTC Interrupt Handler, clears the flag.
 *****************************************************************************/
void RTC_IRQHandler(void)
{
  // Clear interrupt source
  RTC_IntClear(RTC_IFC_COMP0);

  // Start ADC scan
  ADC_Start(ADC0, adcStartScan);
}

void initRtc(void)
{
  // Start LFRCO and wait until it is stable
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

  // Enable clock to the interface of the low energy modules
  CMU_ClockEnable(cmuClock_CORELE, true);
  // Route the LFRCO clock to the RTC
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_RTC, true);

  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  // Set the compare value of the RTC
  RTC_CompareSet(0, WAKEUP_COUNT);

  // Enable RTC interrupt
  RTC_IntEnable(RTC_IEN_COMP0);
  NVIC_ClearPendingIRQ(RTC_IRQn);
  NVIC_EnableIRQ(RTC_IRQn);

  // Initialize the RTC
  RTC_Init(&rtcInit);
}

void initAdc(void)
{
  // Enable clocks required
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef initScan = ADC_INITSCAN_DEFAULT;

  // Modify init structs and initialize
  init.prescale   = ADC_PrescaleCalc(13000000, 0); // Init to max ADC clock for Series 0
  ADC_Init(ADC0, &init);

  initScan.diff = 0; // single-ended
  initScan.reference  = adcRefVDD; // VDD reference
  initScan.resolution = adcRes12Bit;

  // Add CH7 ADC input to scan (see README for corresponding EXP header pin),
  // as well as CH0 just to demonstrate scan mode (may route to any pin)
  initScan.input = ADC_SCANCTRL_INPUTMASK_CH7 | ADC_SCANCTRL_INPUTMASK_CH0;

  ADC_InitScan(ADC0, &initScan);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  initAdc();

  // Set up RTC for 500ms interrupts
  initRtc();

  // Initialize USART, enable board controller VCOM function, and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  // Infinite loop
  while(1)
  {
    // enter EM2 until next RTC interrupt
    EMU_EnterEM2(false);

    // RTC interrupt fires and starts ADC scan

    // Get first scan result
    while(!(ADC0->STATUS & ADC_STATUS_SCANDV)) ;
    ch0Mv = (ADC_DataScanGet(ADC0) * 3300) / 4096;

    // Get second scan result
    while(!(ADC0->STATUS & ADC_STATUS_SCANDV)) ;
    ch7Mv = (ADC_DataScanGet(ADC0) * 3300) / 4096;

    printf("CH7: %dmV, CH0: %dmV\n", (unsigned int) ch7Mv, (unsigned int) ch0Mv);

    // Wait for print to finish by checking the TX Complete (TXC) bit
    while(!(RETARGET_UART->STATUS & UART_STATUS_TXC));
  }

}
