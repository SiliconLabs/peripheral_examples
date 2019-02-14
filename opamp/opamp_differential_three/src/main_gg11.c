/**************************************************************************//**
 * @main_gg11.c
 * @brief This project operates in EM3 and configures opamp 0 as a voltage
 * follower, opamp 1 as a voltage follower, and opamp 2 as a non-inverting
 * opamp.
 *
 * The equation for Vout is shown below (provided that R2/R1 for both opamps
 * are inverses of each other). They have to be inverses because the resistor
 * ladder being used for the input to the positive node of opamp 2 is actually
 * coming from opamp 0 and thus the two resistor ladders are not mirrors of each
 * other. Refer to the application note (AN0038) for an image illustrating this.
 *
 * Vout2 = (Vin0 - Vin1) * (R2 / R1)
 * where Vin0 is the input to the positive node of opamp 0 and
 * Vout2 is the output of opamp 2
 *
 * The generic equation for Vout is shown below:
 *
 * Vout2 = (-Vin1 * (R2_OPA2 / R1_OPA2)) +
 *         (Vin0 * (R1_OPA0 / (R1_OPA0 + R2_OPA0)) * (1 + (R2_OPA2 / R1_OPA2)))
 *
 * By default, this project selects the R2/R1 resistor ladder ratios for both
 * opamps to be inverses such that Vout2 = (Vin0 - Vin1) * 3
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
//       the R2/R1 resistor ladder ratio for opamp 0. By default this
//       is R2 = (1/3) * R1.
#define RESISTOR_SELECT_0 opaResSelR2eq0_33R1

// Note: change this to one of the OPAMP_ResSel_TypeDef type defines to select
//       the R2/R1 resistor ladder ratio for opamp 2. By default this
//       is R2 = 3 * R1.
#define RESISTOR_SELECT_2 opaResSelR2eq3R1

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    No signals are explicitly selected for the negative and positive inputs
 *    of the opamp because the default macro already takes care of setting up
 *    opamp 0 as a voltage follower and opamp 1 as a non-inverting opamp.
 *
 * @note
 *    The code selects VSS as the input to the resistor ladder for opamp 0.
 *    Alternatively, you could select the input to the resistor ladder to be
 *    the negative pad and then connect the negative pad (OPAMP_N0 or OPA0_N)
 *    to ground.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Turn on the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Configure OPA0
  OPAMP_Init_TypeDef init0 = OPA_INIT_DIFF_RECEIVER_OPA0;
  init0.resSel   = RESISTOR_SELECT_0;    // Choose the resistor ladder ratio
  init0.resInMux = opaResInMuxVss;       // Choose VSS as the input to the resistor ladder
  init0.posSel   = opaPosSelAPORT3XCH10; // Choose opamp positive input to come from PE10

  // Configure OPA1
  OPAMP_Init_TypeDef init1 = OPA_INIT_DIFF_RECEIVER_OPA1;
  init1.posSel  = opaPosSelAPORT4XCH11;  // Choose opamp positive input to come from PE11

  // Configure OPA2
  OPAMP_Init_TypeDef init2 = OPA_INIT_DIFF_RECEIVER_OPA2;
  init2.resSel  = RESISTOR_SELECT_2;     // Choose the resistor ladder ratio
  init2.outMode = opaOutModeAPORT4YCH12; // Route opamp output to PE12

  // Enable OPA0, OPA1, OPA2
  OPAMP_Enable(VDAC0, OPA0, &init0);
  OPAMP_Enable(VDAC0, OPA1, &init1);
  OPAMP_Enable(VDAC0, OPA2, &init2);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

