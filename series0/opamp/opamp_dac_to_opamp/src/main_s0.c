/**************************************************************************//**
 * @main_series0.c
 * @brief This project configures opamp 2 as a non-inverting amplifier whose
 * gain is given by the following equation: Vout = Vin * (1 + R2/R1).
 * By default, this project selects the R2/R1 resistor ladder ratio to be
 * R2 = R1. This results in Vout = Vin * 2. This project also configures the
 * DAC to output on Channel 0. The user should use a wire to connect the
 * output of the DAC to the positive input node of the opamp.
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

// Note: change this to change which channel the DAC outputs to. This value can
// be either a zero or one
#define CHANNEL_NUM 0

/**************************************************************************//**
 * @brief
 *    DAC initialization
 *****************************************************************************/
void initDac(void)
{
  // Use default settings
  DAC_Init_TypeDef        init        = DAC_INIT_DEFAULT;
  DAC_InitChannel_TypeDef initChannel = DAC_INITCHANNEL_DEFAULT;

  // Enable the DAC clock
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Calculate the DAC clock prescaler value resulting in a 1 MHz DAC clock.
  init.prescale = DAC_PrescaleCalc(1000000, 0);

  // Set reference to internal 1.25V low noise reference
  init.reference = dacRef1V25;

  // Initialize the DAC and DAC channel
  DAC_Init(DAC0, &init);
  DAC_InitChannel(DAC0, &initChannel, CHANNEL_NUM);

  // Enable the DAC
  DAC_Enable(DAC0, CHANNEL_NUM, true);
}

/**************************************************************************//**
 * @brief
 *    Calculate the digital value that maps to the desired output voltage
 *
 * @note
 *    The vRef parameter must match the reference voltage selected during
 *    initialization
 *
 * @param [in] vOut
 *    Desired output voltage
 *
 * @param [in] vRef
 *    Reference voltage used by the DAC
 *
 * @return
 *    The digital value that maps to the desired output voltage
 *****************************************************************************/
uint32_t getDacValue(float vOut, float vRef)
{
  return (uint32_t)((vOut * 4095) / vRef);
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
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Initialize the ADC and OPAMP
  initDac();
  initOpamp();

  // Output the 12-bit output value for 1.1 V
  uint32_t dacValue = getDacValue(1.1, 1.25);
  DAC_ChannelOutputSet(DAC0, CHANNEL_NUM, dacValue);

  while (1) {
    EMU_EnterEM3(false);
  }
}

