/**************************************************************************//**
 * @main_series0.c
 * @brief  This project operates in EM3 and configures opamps 0, 1, 2 as
 * cascaded inverting amplifiers whose gains are given by the following
 * equations:
 *
 * Vout0 = -(Vin - POS0)   * (R2 / R1) + POS0
 * Vout1 = -(Vout0 - POS1) * (R2 / R1) + POS1
 * Vout2 = -(Vout1 - POS2) * (R2 / R1) + POS2
 *
 * Where POS is the input to the positive terminal of the opamp and Vin is the
 * input to the negative terminal of the opamp. POS usually equals
 * (VS+ + VS-)/2, giving full range on the output.
 *
 * For the EFM32 opamps this equals VDD/2. By default, this project selects the
 * R2/R1 resistor ladder ratio to be R2 = R1.
 *
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
//       the R2/R1 resistor ladder ratio. By default this is R2 = R1.
#define RESISTOR_SELECT opaResSelR2eqR1

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    No signals are explicitly selected for the negative and positive inputs
 *    of the opamp because the default macro already takes care of routing the
 *    negative input to the resistor ladder taps and the positive input to the
 *    positive pad.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Turn on the DAC clock
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Configure OPA0
  OPAMP_Init_TypeDef init0 = OPA_INIT_CASCADED_INVERTING_OPA0;
  init0.resSel = RESISTOR_SELECT;         // Choose the resistor ladder ratio

  // Configure OPA1
  OPAMP_Init_TypeDef init1 = OPA_INIT_CASCADED_INVERTING_OPA1;
  init1.resSel = RESISTOR_SELECT;         // Choose the resistor ladder ratio

  // Configure OPA2
  OPAMP_Init_TypeDef init2 = OPA_INIT_CASCADED_INVERTING_OPA2;
  init2.resSel = RESISTOR_SELECT;         // Choose the resistor ladder ratio
  init2.outPen = DAC_OPA2MUX_OUTPEN_OUT0; // Choose main output location #0

  // Enable OPA0, OPA1, OPA2
  OPAMP_Enable(DAC0, OPA0, &init0);
  OPAMP_Enable(DAC0, OPA1, &init1);
  OPAMP_Enable(DAC0, OPA2, &init2);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

