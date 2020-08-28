/***************************************************************************//**
 * @file main_pg12.c
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

  // Configure OPA0
  OPAMP_Init_TypeDef init0 = OPA_INIT_CASCADED_INVERTING_OPA0;
  init0.resSel   = RESISTOR_SELECT;      // Choose the resistor ladder ratio
  init0.posSel   = opaPosSelAPORT3XCH2;  // Choose opamp positive input to come from PD10
  init0.resInMux = opaResInMuxNegPad;    // Route negative pad to resistor ladder

  // Configure OPA1
  OPAMP_Init_TypeDef init1 = OPA_INIT_CASCADED_INVERTING_OPA1;
  init1.resSel = RESISTOR_SELECT;        // Choose the resistor ladder ratio
  init1.posSel = opaPosSelAPORT4XCH3;    // Choose opamp positive input to come from PD11

  // Configure OPA2
  OPAMP_Init_TypeDef init2 = OPA_INIT_CASCADED_INVERTING_OPA2;
  init2.resSel  = RESISTOR_SELECT;      // Choose the resistor ladder ratio
  init2.posSel  = opaPosSelAPORT1XCH10; // Choose opamp positive input to come from PC10
  init2.outMode = opaOutModeAPORT1YCH9; // Route opamp output to PC9

  // Enable OPA0, OPA1, OPA2
  OPAMP_Enable(VDAC0, OPA0, &init0);
  OPAMP_Enable(VDAC0, OPA1, &init1);
  OPAMP_Enable(VDAC0, OPA2, &init2);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 (won't exit)
  }
}

