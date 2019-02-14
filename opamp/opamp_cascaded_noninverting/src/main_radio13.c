/**************************************************************************//**
 * @main_radio13.c
 * @brief This project operates in EM3 and configures opamp 0, 1, 2 as
 * non-inverting amplifiers whose gains are given by the following equations:
 *
 * Vout0 = Vin0  * (1 + R2/R1)
 * Vout1 = Vout0 * (1 + R2/R1) = Vin0 * ((1 + R2/R1) ^ 2)
 * Vout2 = Vout1 * (1 + R2/R1) = Vin0 * ((1 + R2/R1) ^ 3)
 *
 * By default, this project selects the R2/R1 resistor
 * ladder ratio to be R2 = R1. This results in
 *
 * Vout0 = Vin0 * 2
 * Vout1 = Vout0 * 2 = Vin0 * 4
 * Vout2 = Vout1 * 2 = Vin0 * 8
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
//       the R2/R1 resistor ladder ratio. By default this is R2 = R1
#define RESISTOR_SELECT opaResSelR2eqR1

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    No signals are explicitly selected for the negative input of the opamp
 *    because the default macro already takes care of routing the resistor
 *    ladder tap to the negative input.
 *
 * @note
 *    The input to the resistor ladder must be set to ground for a non-inverting
 *    opamp configuration (either by setting RESINMUX to VSS or by setting
 *    RESINMUX to NEGPAD and then connecting the negative pad to ground.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Turn on the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Configure OPA0
  OPAMP_Init_TypeDef init0 = OPA_INIT_CASCADED_NON_INVERTING_OPA0;
  init0.resSel   = RESISTOR_SELECT;      // Choose the resistor ladder ratio
  init0.resInMux = opaResInMuxVss;       // Set the input to the resistor ladder to VSS
  init0.posSel   = opaPosSelAPORT1XCH6;  // Choose opamp positive input to come from PC6

  // Configure OPA1
  OPAMP_Init_TypeDef init1 = OPA_INIT_CASCADED_NON_INVERTING_OPA1;
  init1.resSel   = RESISTOR_SELECT;      // Choose the resistor ladder ratio
  init1.resInMux = opaResInMuxVss;       // Set the input to the resistor ladder to VSS
  init1.posSel   = opaPosSelOpaIn;       // Choose opamp positive input to come from OPA0

  // Configure OPA2
  OPAMP_Init_TypeDef init2 = OPA_INIT_CASCADED_NON_INVERTING_OPA2;
  init2.resSel   = RESISTOR_SELECT;      // Choose the resistor ladder ratio
  init2.resInMux = opaResInMuxVss;       // Set the input to the resistor ladder to VSS
  init2.posSel   = opaPosSelOpaIn;       // Choose opamp positive input to come from OPA1
  init2.outMode  = opaOutModeAPORT1YCH7; // Route opamp output to PC7

  // Enable OPA0, OPA1, OPA2
  OPAMP_Enable(VDAC0, OPA0, &init0);
  OPAMP_Enable(VDAC0, OPA1, &init1);
  OPAMP_Enable(VDAC0, OPA2, &init2);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

