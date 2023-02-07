/***************************************************************************//**
 * @file main_vdac_differential.c
 * @brief This project uses the VDAC in continuous mode with differential output
 * to output a difference of 0.5V between two pins in EM3. See readme.txt for
 * details.
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
#include "em_gpio.h"
#include "mx25flash_spi.h"

// Set the VDAC to max frequency of 1 MHz
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
 * used. Refer to the device Reference Manual and Datasheet for more details.
 * We will be selecting the CH0 main output PB00 and CH1 main output PB001 for
 * this example.
 */

/**************************************************************************//**
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
 *****************************************************************************/
void powerDownSpiFlash(void)
{
  FlashStatus status;

  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();
}

/**************************************************************************//**
 * @brief
 *    VDAC initialization
 *****************************************************************************/
void initVdac(void)
{
  // Use default settings
  VDAC_Init_TypeDef        init        = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

  // Use the HFRCOEM23 to clock the VDAC in order to operate in EM3 mode
  CMU_ClockSelectSet(cmuClock_VDAC0, cmuSelect_HFRCOEM23);

  // Enable the HFRCOEM23 and VDAC clocks
  CMU_ClockEnable(cmuClock_HFRCOEM23, true);
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // Calculate the VDAC clock prescaler value resulting in a 1 MHz VDAC clock
  init.prescaler = VDAC_PrescaleCalc(VDAC0, CLK_VDAC_FREQ);

  // Set the output mode to differential instead of single-ended
  init.diff = true;

  // Clocking is requested on demand
  init.onDemandClk = false;

  // Disable High Capacitance Load mode
  initChannel.highCapLoadEnable = false;

  // Use Low Power mode
  initChannel.powerMode = vdacPowerModeLowPower;

  // Initialize the VDAC and VDAC channel
  VDAC_Init(VDAC0, &init);
  VDAC_InitChannel(VDAC0, &initChannel, 0);
  VDAC_InitChannel(VDAC0, &initChannel, 1);

  // Enable the VDAC
  VDAC_Enable(VDAC0, 0, true);
  VDAC_Enable(VDAC0, 1, true);
}

/**************************************************************************//**
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
 *****************************************************************************/
uint32_t getVdacValue(float vOut, float vRef)
{
  return (uint32_t)((vOut * 2047) / vRef);
}

/**************************************************************************//**
 * @brief
 *****************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;

  // Chip errata
  CHIP_Init();

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
  VDAC_ChannelOutputSet(VDAC0, 0, vdacValue);

  while (1) {
    // Enter EM3 while the VDAC is doing continuous conversions
    EMU_EnterEM3(false);
  }
}
