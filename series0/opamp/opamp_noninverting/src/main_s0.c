/**************************************************************************//**
 * @main_series0.c
 * @brief This project operates in EM3 and configures opamp 2 as a
 * non-inverting amplifier whose gain is given by the following equation:
 * Vout = Vin * (1 + R2/R1). By default, this project selects the R2/R1 resistor
 * ladder ratio to be R2 = R1. This results in Vout = Vin * 2.
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

// Note: change this to one of the OPAMP_ResSel_TypeDef type defines to select
//       the R2/R1 resistor ladder ratio. By default this is R2 = R1. This
//       results in Vout = Vin * 2
#define RESISTOR_SELECT opaResSelR2eqR1

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

  // Turn on the DAC clock
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Configure OPA2
  OPAMP_Init_TypeDef init = OPA_INIT_NON_INVERTING_OPA2;
  init.resInMux = opaResInMuxVss;          // Set the input to the resistor ladder to VSS
  init.resSel   = RESISTOR_SELECT;         // Choose the resistor ladder ratio
  init.outPen   = DAC_OPA2MUX_OUTPEN_OUT0; // Choose main output location #0

  // Enable OPA2
  OPAMP_Enable(DAC0, OPA2, &init);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

