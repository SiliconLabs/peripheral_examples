/**************************************************************************//**
 * @main_series0.c
 * @brief This project operates in EM3 and configures opamp 0 as a voltage
 * follower and opamp 1 as a non-inverting opamp. The equation for Vdiff is
 * shown below:
 *
 * Vdiff = (Vin1 - Vin0) * (R2 / R1)
 * Vdiff = Vout1 - Vin1
 * where Vin0 is the input to the positive node of opamp 0 and
 * Vout1 is the output of opamp 1
 *
 * By default, this project selects the R2/R1 resistor
 * ladder ratio to be R2 = 3 * R1. This results in
 * Vdiff = (Vin1 - Vin0) * 3
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
//       the R2/R1 resistor ladder ratio. By default this is R2 = 3 * R1.
#define RESISTOR_SELECT opaResSelR2eq3R1 

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    No signals are explicitly selected for the negative and positive inputs
 *    of the opamp because the default macro already takes care of setting up
 *    opamp 0 as a voltage follower and opamp 1 as a non-inverting opamp. The
 *    macro also sets the outPen field to 0 to disable alternate outputs for
 *    OPA0.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Turn on the DAC clock
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Configure OPA0
  OPAMP_Init_TypeDef init0 = OPA_INIT_DIFF_DRIVER_OPA0;

  // Configure OPA1
  OPAMP_Init_TypeDef init1 = OPA_INIT_DIFF_DRIVER_OPA1;
  init1.resSel = RESISTOR_SELECT;         // Choose the resistor ladder ratio
  init1.outPen = DAC_OPA1MUX_OUTPEN_OUT0; // Choose main output location #0

  // Enable OPA0 and OPA1
  OPAMP_Enable(DAC0, OPA0, &init0);
  OPAMP_Enable(DAC0, OPA1, &init1);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

