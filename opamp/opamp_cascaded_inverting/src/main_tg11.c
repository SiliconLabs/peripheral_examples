/**************************************************************************//**
 * @main_tg11.c
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
 *    No signal is explicitly selected for the negative input of the opamp
 *    because the default macro already takes care of routing it to the resistor
 *    ladder taps. The input to the resistor ladder was chosen to come from the
 *    negative pad (OPA0_N), but the positive pad (OPA0_P) could also be chosen.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Turn on the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Configure OPA1
  OPAMP_Init_TypeDef init1 = OPA_INIT_CASCADED_INVERTING_OPA1;
  init1.resSel   = RESISTOR_SELECT;      // Choose the resistor ladder ratio
  init1.resInMux = opaResInMuxNegPad;    // Route negative pad to resistor ladder

  // Configure OPA2
  OPAMP_Init_TypeDef init2 = OPA_INIT_CASCADED_INVERTING_OPA2;
  init2.resSel = RESISTOR_SELECT;        // Choose the resistor ladder ratio
  init2.posSel = opaPosSelAPORT1XCH14;   // Choose opamp positive input to come from PA14

  // Configure OPA3
  OPAMP_Init_TypeDef init3 = OPA_INIT_CASCADED_INVERTING_OPA3;
  init3.resSel  = RESISTOR_SELECT;       // Choose the resistor ladder ratio
  init3.posSel  = opaPosSelAPORT3XCH16;  // Choose opamp positive input to come from PF0
  init3.outMode = opaOutModeAPORT3YCH17; // Route opamp output to PF1

  // Enable OPA1, OPA2, OPA3
  OPAMP_Enable(VDAC0, OPA1, &init1);
  OPAMP_Enable(VDAC0, OPA2, &init2);
  OPAMP_Enable(VDAC0, OPA3, &init3);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

