/***************************************************************************//**
 * @file main_s0.c
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

