/***************************************************************************//**
 * @file main_vdac_sine_wave_synch.c
 * @brief This project uses the DAC and the internal sine wave generator to
 * produce a 16-point sine wave at a frequency of f_sinewave Hz centered at the
 * reference voltage divided by 2. This project operates in EM1. By default,
 * this program outputs a sine wave at 31,250 Hz.
 * f_sinewave = f_HFPERCLK / (32 * (PRESCALE + 1))
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_vdac.h"

// Note: The sine wave will always be output on Channel 0 but Channel 1 can
// still be used independently as a single-ended DAC output. Channel 1 settings
// are ignored if the differential mode is enabled.
#define CHANNEL_NUM 0

// Set the VDAC to max frequency of 1 MHz
// Note in this example: f_sinewave = f_EM01GRPACLK / (32 * (PRESCALE + 1))
// The default value of the f_EM01GRPACLK is 19 MHz and PRESCALE is 18 for the
// fastest VDAC clock which makes f_sinewave ~ 31 kHz
#define CLK_VDAC_FREQ              1000000

/*
 * The port and pin for the VDAC output is set in VDAC_OUTCTRL register. The
 * ABUSPORTSELCHx fields in this register are defined as follows:
 * 0  No GPIO selected for CHx ABUS output
 * 1  Port A selected
 * 2  Port B selected
 * 3  Port C selected
 * 4  Port D selected
 *
 * The VDAC port pin settings do not need to be set when the main output is
 * used. Refer to the device Reference Manual and Datasheet for more details. We
 * will be selecting the CH0 main output PB00 for this example.
 */

/**************************************************************************//**
 * @brief
 *    VDAC initialization
 *****************************************************************************/
void initVdac(void)
{
  // Use default settings
  VDAC_Init_TypeDef        init        = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

  // Sine mode is supported only for the fastest configuration of the VDAC in
  // continuous mode. Hence select the EM01GRPACLK as the source of the VDAC0
  CMU_ClockSelectSet(cmuClock_VDAC0, cmuSelect_EM01GRPACLK);

  // Enable the VDAC clocks
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Calculate the VDAC clock prescaler value resulting in a 1 MHz VDAC clock.
  init.prescaler = VDAC_PrescaleCalc(VDAC0, (uint32_t)CLK_VDAC_FREQ);

  // Set reference to internal 1.25V reference
  init.reference = vdacRef1V25;

  // Enable sine mode
  init.sineEnable = true;

  // Set the output mode to continuous as required for the sine mode
  initChannel.sampleOffMode = false;

  // Since the minimum load requirement for high capacitance mode is 25 nF, turn
  // this mode off
  initChannel.highCapLoadEnable = false;

  // Trigger mode and refresh source should be programmed to None for the sine
  // mode to avoid interference in sine output generation from other triggers
  initChannel.trigMode = vdacTrigModeNone;
  initChannel.chRefreshSource = vdacRefreshSrcNone;

  // Initialize the VDAC and VDAC channel
  VDAC_Init(VDAC0, &init);
  VDAC_InitChannel(VDAC0, &initChannel, CHANNEL_NUM);

  // Enable the VDAC
  VDAC_Enable(VDAC0, CHANNEL_NUM, true);
}

/**************************************************************************//**
 * @brief
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;

  // Enable DC-DC converter
  EMU_DCDCInit(&dcdcInit);

  // Initialize the VDAC
  initVdac();

  // Start sine mode
  VDAC_SineModeStart(VDAC0, true);

  while (1) {
    // Enter EM1 (won't exit)
    EMU_EnterEM1();
  }
}

