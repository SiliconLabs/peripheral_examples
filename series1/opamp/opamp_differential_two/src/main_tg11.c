/**************************************************************************//**
 * @main_gg11.c
 * @brief This project operates in EM3 and configures opamp 1 as a voltage
 * follower and opamp 2 as a non-inverting opamp. The equation for Vdiff is
 * shown below:
 *
 * Vdiff = (Vin2 - Vin1) * (R2 / R1)
 * Vdiff = Vout2 - Vin2
 * where Vin1 is the input to the positive node of opamp 1 and
 * Vout2 is the output of opamp 2
 *
 * By default, this project selects the R2/R1 resistor
 * ladder ratio to be R2 = 3 * R1. This results in
 * Vdiff = (Vin2 - Vin1) * 3
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
 *    The default macro sets up most of the configuration, but the code chooses
 *    to use the APORT for more flexible pin routing.
 *
 * @note
 *    The default OPA_INIT_DIFF_DRIVER_OPA0 macro is used for OPA1 because 
 *    opamp 1 is the opamp configured as the voltage follower. Similarly, the
 *    OPA_INIT_DIFF_DRIVER_OPA1 is used for OPA2 because opamp 2 is the opamp
 *    configured as the non-inverting opamp
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Turn on the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Configure OPA1
  OPAMP_Init_TypeDef init1 = OPA_INIT_DIFF_DRIVER_OPA0;

  // Configure OPA2
  OPAMP_Init_TypeDef init2 = OPA_INIT_DIFF_DRIVER_OPA1;
  init2.posSel = opaPosSelAPORT1XCH14;  // Choose opamp positive input to come from PA14
  init2.resSel = RESISTOR_SELECT;       // Choose the resistor ladder ratio

  // Enable OPA1 and OPA2
  OPAMP_Enable(VDAC0, OPA1, &init1);
  OPAMP_Enable(VDAC0, OPA2, &init2);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

