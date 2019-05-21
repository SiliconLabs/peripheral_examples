/**************************************************************************//**
 * @main_radio14.c
 * @brief This project configures opamp 0 as a non-inverting amplifier whose
 * gain is given by the following equation: Vout = Vin * (1 + R2/R1).
 * By default, this project selects the R2/R1 resistor
 * ladder ratio to be R2 = R1. This results in Vout = Vin * 2. This project also
 * configures the ADC to receive input on the same pin the opamp is outputting
 * to so that the user can enter debug mode to check the ADC conversion of the
 * opamp output.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_opamp.h"
#include "em_adc.h"

// Note: change this to one of the OPAMP_ResSel_TypeDef type defines to select
//       the R2/R1 resistor ladder ratio. By default this is R2 = R1. This
//       results in Vout = Vin * 2
#define RESISTOR_SELECT opaResSelR2eqR1

// Note: These aren't necessary and are only provided so that they can be viewed
// in the debugger. Additionally they are declared as volatile so that they
// won't be optimized out by the compiler
static volatile uint32_t sample;
static volatile uint32_t millivolts;

/**************************************************************************//**
 * @brief
 *    Initialize ADC
 *****************************************************************************/
void initAdc(void)
{
  // Enable ADC clock
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Initialize the ADC
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  init.timebase = ADC_TimebaseCalc(0);           // Make sure timebase is at least 1 microsecond
  init.prescale = ADC_PrescaleCalc(16000000, 0); // Init to max ADC clock for Series 1
  ADC_Init(ADC0, &init);

  // Set the ADC to use single ended mode
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;
  initSingle.diff       = 0;                   // Single ended
  initSingle.reference  = adcRef2V5;           // Internal 2.5V reference
  initSingle.resolution = adcRes12Bit;         // 12-bit resolution
  initSingle.posSel     = adcPosSelAPORT1YCH7; // Choose input to ADC to be on PC7
  ADC_InitSingle(ADC0, &initSingle);
}

/**************************************************************************//**
 * @brief
 *    Initialize OPAMP
 *****************************************************************************/
void initOpamp(void)
{
  // Configure OPA0
  OPAMP_Init_TypeDef init = OPA_INIT_NON_INVERTING;
  init.resInMux = opaResInMuxVss;       // Set the input to the resistor ladder to VSS
  init.resSel   = RESISTOR_SELECT;      // Choose the resistor ladder ratio
  init.posSel   = opaPosSelAPORT1XCH6;  // Choose opamp positive input to come from PC6
  init.outMode  = opaOutModeAPORT1YCH7; // Route opamp output to PC7

  // Enable OPA0
  OPAMP_Enable(VDAC0, OPA0, &init);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    No signals are explicitly selected for the negative and positive inputs
 *    of the opamp because the default OPA_INIT_NON_INVERTING_OPA0 macro
 *    already takes care of routing the resistor ladder tap to the negative
 *    input and since there is only one available choice for the positive
 *    input it also takes care of that already.
 *
 * @note
 *    The input to the resistor ladder must be set to ground for a non-inverting
 *    opamp configuration (either by setting RESINMUX to VSS or by setting
 *    RESINMUX to NEGPAD and then connecting the negative pad (in this case
 *    OPAMP_N0) to ground.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Enable the VDAC clock for accessing the opamp registers
  CMU_ClockEnable(cmuClock_VDAC0, true); // Enable VDAC clock

  // Initialize the ADC and OPAMP
  initAdc();
  initOpamp();

  while (1) {

    // Start the ADC conversion
    ADC_Start(ADC0, adcStartSingle);

    // Wait for conversion to complete
    while(!(ADC0->STATUS & _ADC_STATUS_SINGLEDV_MASK));

    // Get ADC result
    sample = ADC_DataSingleGet(ADC0);

    // Calculate input voltage in mV
    // mV = (sample * 2.5Vref * 1000) / (2^12 bit resolution)
    millivolts = (sample * 2500) / 4096;
  }
}

