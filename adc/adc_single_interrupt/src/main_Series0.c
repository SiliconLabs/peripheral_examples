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
#include "bsp.h"
#include "retargetserial.h"
#include "em_adc.h"

uint32_t sample;
uint32_t millivolts;
volatile uint32_t msTicks; // counts 1ms timeTicks

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       // increment counter necessary in Delay()
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

void initADC (void)
{
  // Enable clocks required
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

  // Modify init structs and initialize
  init.prescale   = ADC_PrescaleCalc(13000000, 0); // Init to max ADC clock for Series 0
  ADC_Init(ADC0, &init);

  initSingle.diff       = 0; // single ended
  initSingle.reference  = adcRef2V5; // internal 2.5V reference

  // Select ADC input. See README for corresponding EXP header pin.
  initSingle.input = adcSingleInputCh7;

  initSingle.resolution = adcRes12Bit;
  ADC_InitSingle(ADC0, &initSingle);

  // Enable ADC Single Conversion Complete interrupt
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
  ADC_IntEnable(ADC0, ADC_IF_SINGLE);
}

void ADC0_IRQHandler(void)
{
  // Clear the interrupt flag
  ADC_IntClear(ADC0, ADC_IF_SINGLE);

  // Get ADC result
  sample = ADC_DataSingleGet(ADC0);

  // Calculate input voltage in mV
  millivolts = (sample * 2500) / 4096;

  // Start next ADC conversion
  ADC_Start(ADC0, adcStartSingle);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  initADC();

  ADC_Start(ADC0, adcStartSingle); // start first conversion

  // Initialize USART, enable board controller VCOM function, and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  // Setup SysTick Timer for 1 msec interrupts
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

  // Infinite loop
  while(1)
  {
    Delay(500);
    printf("%dmV\n", (unsigned int) millivolts);
  }

}
