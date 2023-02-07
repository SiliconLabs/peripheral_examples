/***************************************************************************//**
 * @file main_vdac_sample_off.c
 * @brief This project uses the VDAC in sample/off mode to output 0.5V to a pin
 * in EM3. See readme.txt for details.
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
#include "mx25flash_spi.h"

// Note: change this to change which channel the VDAC outputs to. This value can
// be either a zero or one
#define CHANNEL_NUM 0

// The DAC clock speed is set to 32.768 kHz. The clock source for the DAC in
// this example is the EM23GRPACLK, sourced by default by 32.768 kHz LFRCO.
#define CLK_VDAC_FREQ             32768

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

/***************************************************************************//**
 * @brief
 *    Powers down the SPI flash on the radio board
 *
 * @details
 *    A JEDEC standard SPI flash boots up in standby mode in order to
 *    provide immediate access, such as when used it as a boot memory.
 *
 *    Typical current draw in standby mode for the MX25R8035F device used
 *    on EFR32 radio boards is 5 µA.
 *
 *    JEDEC standard SPI flash memories have a lower current deep power-down
 *    mode, which can be entered after sending the relevant commands. This is on
 *    the order of 0.007 µA for the MX25R8035F.
 ******************************************************************************/
void powerDownSpiFlash(void)
{
  FlashStatus status;

  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();
}

/***************************************************************************//**
 * @brief
 *    VDAC initialization
 ******************************************************************************/
void initVdac(void)
{
  // Use default settings
  VDAC_Init_TypeDef        init        = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

  // Since this example showcases the low power mode of the VDAC, the device is
  // clocked off the EM23GRPACLK. This clock is recommended only when the VDAC
  // is expected to do very slow sample conversions/refresh.
  CMU_ClockSelectSet(cmuClock_VDAC0, cmuSelect_EM23GRPACLK);

  // Enable the VDAC clocks
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Calculate the VDAC prescaler value resulting in a 32.768 kHz VDAC clock.
  init.prescaler = VDAC_PrescaleCalc(VDAC0, (uint32_t)CLK_VDAC_FREQ);

  // Set reference to internal 1.25V low noise reference
  init.reference = vdacRef1V25;

  /* When using EM23GRPACLK, the clock source cannot be made "on demand";
   * Setting this bool/bitfield to true/logic 1 disables "on demand"
   */
  init.onDemandClk = true;

  // Set VDAC channel refresh period
  init.refresh = vdacRefresh32;

  // Since the VDAC runs in EM3, low power mode should be enabled
  initChannel.powerMode = vdacPowerModeLowPower;

  // Set the output mode to sample/off. In sample/off mode, the VDAC will only
  // drive the output for a limited time per conversion.
  initChannel.sampleOffMode = true;

  // The VDAC will drive the output for 10 prescaled VDAC clock cycles before
  // tri-stating the output again
  initChannel.holdOutTime = 10;

  // Set the trigger mode to SW so that the internal timer or a software
  // trigger will trigger the VDAC conversion
  initChannel.trigMode = vdacTrigModeSw;

  // A conversion will start on an overflow of the refresh timer. This is an
  // internal low power refresh timer that is automatically started. It will
  // count the number of clock refresh cycles programmed in the init
  // configuration before wrapping and generating a refresh trigger.
  initChannel.chRefreshSource = vdacRefreshSrcRefreshTimer;

  // Since the minimum load requirement for high capacitance mode is 25 nF, turn
  // this mode off
  initChannel.highCapLoadEnable = false;

  // Initialize the VDAC and VDAC channel
  VDAC_Init(VDAC0, &init);
  VDAC_InitChannel(VDAC0, &initChannel, CHANNEL_NUM);

  // Enable the VDAC
  VDAC_Enable(VDAC0, CHANNEL_NUM, true);
}

/***************************************************************************//**
 * @brief
 *    Calculate the digital value that maps to the desired output voltage
 *
 * @note
 *    The vRef parameter must match the reference voltage selected during
 *    initialization
 *
 * @param [in] vOut
 *    Desired output voltage
 *
 * @param [in] vRef
 *    Reference voltage used by the VDAC
 *
 * @return
 *    The digital value that maps to the desired output voltage
 ******************************************************************************/
uint32_t getVdacValue(float vOut, float vRef)
{
  return (uint32_t)((vOut * 4095) / vRef);
}

/***************************************************************************//**
 * @brief
 *    Output 0.5 volts to VDAC channel 0
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;

  // Enable DC-DC converter
  EMU_DCDCInit(&dcdcInit);

  // Enable voltage downscaling in energy modes EM2 and EM3
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;

  // Initialize energy modes EM2 and EM3
  EMU_EM23Init(&em23Init);

  // Power down the SPI flash
  powerDownSpiFlash();

  // Initialize the VDAC
  initVdac();

  // Calculate the 12-bit output value for 0.5 V
  uint32_t vdacValue = getVdacValue(0.5, 1.25);

  // Write the output value to VDAC DATA register
  VDAC_ChannelOutputSet(VDAC0, CHANNEL_NUM, vdacValue);

  while (1) {
      // Enter EM3 while the VDAC is doing continuous conversions
    EMU_EnterEM3(false);
  }
}

