/**************************************************************************//**
 * @file
 * @brief Trigger single ADC conversions periodically via LETIMER
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
#include "em_adc.h"
#include "em_gpio.h"
#include "em_letimer.h"

// Init to max ADC clock for Series 0
#define adcFreq         13000000
#define letimerClkFreq  14000000

// Desired letimer interrupt frequency (in Hz)
#define letimerDesired  1000

#define letimerCompare  letimerClkFreq / letimerDesired

volatile uint32_t sample;
volatile uint32_t millivolts;

/**************************************************************************//**
 * @brief LETIMER initialization
 *****************************************************************************/
void initLETIMER(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Enable clock to the LE modules interface
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Select LFXO for the LETIMER
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LETIMER0, true);

  // Reload COMP0 on underflow, idle output, and run in repeat mode
  letimerInit.comp0Top  = true;
  letimerInit.ufoa0     = letimerUFOANone;
  letimerInit.repMode   = letimerRepeatFree;

  // Need REP0 != 0 to pulse on underflow
  LETIMER_RepeatSet(LETIMER0, 0, 1);

  // Compare on wake-up interval count
  LETIMER_CompareSet(LETIMER0, 0, letimerCompare);

  // Initialize and enable LETIMER
  LETIMER_Init(LETIMER0, &letimerInit);

  // Enable LETIMER0 interrupts for COMP0
  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP0);

  // Enable LETIMER interrupts
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);
}

/**************************************************************************//**
 * @brief ADC initialization
 *****************************************************************************/
void initADC (void)
{
  // Enable clocks required
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

  // Modify init structs
  init.prescale   = ADC_PrescaleCalc(adcFreq, 0);
  init.timebase   = ADC_TimebaseCalc(0);

  initSingle.diff       = false;       // single ended
  initSingle.reference  = adcRef2V5;   // internal 2.5V reference
  initSingle.resolution = adcRes12Bit; // 12-bit resolution

  // Select ADC input. See README for corresponding EXP header pin.
  initSingle.input = adcSingleInputCh7;

  // Initialize ADC and Single conversions
  ADC_Init(ADC0, &init);
  ADC_InitSingle(ADC0, &initSingle);

  // Enable ADC Single Conversion Complete interrupt
  ADC_IntEnable(ADC0, ADC_IEN_SINGLE);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
}

/**************************************************************************//**
 * @brief LETIMER Handler
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
  // Clear the interrupt flag
  LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP0);

  // Start next ADC conversion
  ADC_Start(ADC0, adcStartSingle);
}

/**************************************************************************//**
 * @brief ADC Handler
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
  // Clear interrupt
  ADC_IntClear(ADC0, ADC_IFC_SINGLE);

  // Get ADC result
  sample = ADC_DataSingleGet(ADC0);

  // Calculate input voltage in mV
  millivolts = (sample * 2500) / 4096;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  initADC();

  initLETIMER();

  // Infinite loop
  while(1);
}
