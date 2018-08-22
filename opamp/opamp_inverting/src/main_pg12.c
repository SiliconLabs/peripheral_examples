/**************************************************************************//**
 * @main_pg12.c
 * @brief This project operates in EM3 and configures opamp 2 as an
 * inverting amplifier whose gain is given by the following equation:
 *
 * Vout = -(Vin - POS) * (R2 / R1) + POS.
 *
 * By default, this project selects the R2/R1 resistor ladder ratio to
 * be R2 = 3 * R1. This results in Vout = -3 * (Vin - POS) + POS = -3Vin + 4POS
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
//       the R2/R1 resistor ladder ratio. By default this is R2 = 3 * R1. This
//       results in Vout = -3Vin + 4POS
#define RESISTOR_SELECT opaResSelR2eq3R1

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    No signal is explicitly selected for the negative input of the opamp
 *    because the default macro already takes care of routing it to the resistor
 *    ladder taps. The input to the resistor ladder was chosen to come from the
 *    negative pad (OPA2_N), but the positive pad (OPA2_P) could also be chosen.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Turn on the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Configure OPA2
  OPAMP_Init_TypeDef init = OPA_INIT_INVERTING;
  init.resSel   = RESISTOR_SELECT;      // Choose the resistor ladder ratio
  init.posSel   = opaPosSelAPORT3XCH2;  // Choose opamp positive input to come from PD10
  init.outMode  = opaOutModeAPORT3YCH3; // Route opamp output to PD11
  init.resInMux = opaResInMuxNegPad;    // Route negative pad to resistor ladder

  // Enable OPA2
  OPAMP_Enable(VDAC0, OPA2, &init);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

