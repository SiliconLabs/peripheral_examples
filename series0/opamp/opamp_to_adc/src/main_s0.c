/***************************************************************************//**
 * @file main_s0.c
 * @brief This project configures opamp 2 as a non-inverting amplifier whose
 * gain is given by the following equation: Vout = Vin * (1 + R2/R1).
 * By default, this project selects the R2/R1 resistor
 * ladder ratio to be R2 = R1. This results in Vout = Vin * 2. This project also
 * configures the ADC to receive input on the same pin the opamp is outputting
 * to so that the user can enter debug mode to check the ADC conversion of the
 * opamp output.
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
  init.prescale = ADC_PrescaleCalc(13000000, 0); // Init to max ADC clock for Series 0
  ADC_Init(ADC0, &init);

  // Set the ADC to use single ended mode
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;
  initSingle.diff       = 0;                 // Single ended
  initSingle.reference  = adcRef2V5;         // Internal 2.5V reference
  initSingle.resolution = adcRes12Bit;       // 12-bit resolution
  initSingle.input      = adcSingleInputCh5; // Choose input to ADC to be on PD5
  ADC_InitSingle(ADC0, &initSingle);
}

/**************************************************************************//**
 * @brief
 *    Initialize OPAMP
 *****************************************************************************/
void initOpamp(void)
{
  // Configure OPA2
  OPAMP_Init_TypeDef init = OPA_INIT_NON_INVERTING_OPA2;
  init.resInMux = opaResInMuxVss;          // Set the input to the resistor ladder to VSS
  init.resSel   = RESISTOR_SELECT;         // Choose the resistor ladder ratio
  init.outPen   = DAC_OPA2MUX_OUTPEN_OUT0; // Choose main output location #0 (PD5)

  // Enable OPA2
  OPAMP_Enable(DAC0, OPA2, &init);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    No signals are explicitly selected for the negative and positive inputs
 *    of the opamp because the default OPA_INIT_NON_INVERTING_OPA2 macro
 *    already takes care of routing the resistor ladder tap to the negative
 *    input and since there is only one available choice for the positive
 *    input it also takes care of that already.
 *
 * @note
 *    The input to the resistor ladder must be set to ground for a non-inverting
 *    opamp configuration (either by setting RESINMUX to VSS or by setting
 *    RESINMUX to NEGPAD and then connecting the negative pad (in this case
 *    OPAMP_N2) to ground.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Enable the DAC clock for accessing the opamp registers
  CMU_ClockEnable(cmuClock_DAC0, true); // Enable DAC clock

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

