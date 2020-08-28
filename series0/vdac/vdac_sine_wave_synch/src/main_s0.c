/***************************************************************************//**
 * @file main_s0.c
 * @brief This project uses the DAC and the internal sine wave generator to
 * produce a 16-point sine wave at a frequency of f_sinewave Hz centered at the
 * reference voltage divided by 2. This project operates in EM1. By default,
 * this program outputs a sinewave at 27.343 kHz
 * f_sinewave = f_HFPERCLK / (32 * (2 ^ PRESCALE))
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
#include "em_dac.h"
#include "em_prs.h"

// Note: change this to change the frequency of the sinewave
// Note: PRESCALE must be greater than or equal to 4 because the DAC_CLK can only be programmed to a maximum of 1 MHz.
// Note: PRESCALE must also be less than or equal to 7 because the PRESCALE is a 3 bit unsigned integer
// f_DAC_CLK = f_IN_CLK / (2 ^ PRESCALE), where f_IN_CLK in this example is the HFPERCLK (14 MHz)
// f_sinewave = f_HFPERCLK / (32 * (2 ^ PRESCALE))
#define PRESCALE 4

/**************************************************************************//**
 * @brief
 *    PRS initialization
 *
 * @details
 *    PRS channel inputting to DAC channel 0 controls the sinewave enable.
 *    PRS channel inputting to DAC channel 1 controls the output enable.
 *    This example maps PRS channel 0 to DAC channel 0 and PRS channel 1 to DAC
 *    channel 1. Setting PRS channel 0 and 1 high both enables the sinewave and
 *    enables DAC output.
 *
 * @note
 *    The channels chosen when setting their levels must match the channels that
 *    were mapped to the DAC channels chosen in initDac().
 *****************************************************************************/
void initPrs(void) 
{
  // Enable the PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Set the level for PRS channel 0 and 1 to be high
  PRS_LevelSet(PRS_SWLEVEL_CH0LEVEL | PRS_SWLEVEL_CH1LEVEL,
               _PRS_SWLEVEL_CH0LEVEL_MASK | _PRS_SWLEVEL_CH1LEVEL_MASK);
}

/**************************************************************************//**
 * @brief
 *    DAC initialization
 *****************************************************************************/
void initDac(void)
{
  // Enable the DAC clock
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Initialize the DAC
  DAC_Init_TypeDef init = DAC_INIT_DEFAULT;
  init.prescale         = PRESCALE;   // Set the prescaler
  init.reference        = dacRef1V25; // Set reference to internal 1.25V low noise
  init.sineEnable       = true;       // Enable sine wave generation mode
  init.outEnablePRS     = true;       // Enable PRS control of output driver
  DAC_Init(DAC0, &init);

  // Initialize DAC channel 0 and enable it
  DAC_InitChannel_TypeDef initChannel0 = DAC_INITCHANNEL_DEFAULT;
  initChannel0.prsEnable = true;         // Enable PRS trigger
  initChannel0.prsSel    = dacPRSSELCh0; // Select PRS channel 0 for DAC channel 0
  DAC_InitChannel(DAC0, &initChannel0, 0);

  // Initialize DAC channel 1 and enable it
  DAC_InitChannel_TypeDef initChannel1 = DAC_INITCHANNEL_DEFAULT;
  initChannel1.prsEnable = true;         // Enable PRS trigger
  initChannel1.prsSel    = dacPRSSELCh1; // Select PRS channel 1 for DAC channel 1
  DAC_InitChannel(DAC0, &initChannel1, 1);

  // Enable DAC channels 0 and 1
  DAC_Enable(DAC0, 0, true);
  DAC_Enable(DAC0, 1, true);
}

/**************************************************************************//**
 * @brief
 *    Output a sine wave to DAC channel 0
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialization
  initPrs();
  initDac();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

