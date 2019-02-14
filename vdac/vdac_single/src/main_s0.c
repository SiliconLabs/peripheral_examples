/**************************************************************************//**
 * @file main_series0.c
 * @brief This project uses the DAC in sample/off mode to output 0.5V to a pin
 * in EM3. See readme.txt for details.
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
#include "em_dac.h"

// Note: change this to change which channel the DAC outputs to. This value can
// be either a zero or one
#define CHANNEL_NUM 0

/**************************************************************************//**
 * @brief
 *    DAC initialization
 *****************************************************************************/
void initDac(void)
{
  // Use default settings
  DAC_Init_TypeDef        init        = DAC_INIT_DEFAULT;
  DAC_InitChannel_TypeDef initChannel = DAC_INITCHANNEL_DEFAULT;

  // Enable the DAC clock
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Calculate the DAC clock prescaler value resulting in a 1 MHz DAC clock.
  init.prescale = DAC_PrescaleCalc(1000000, 0);

  // Set reference to internal 1.25V low noise reference
  init.reference = dacRef1V25;

  // Set the refresh period to every 8 clock cycles
  init.refresh = dacRefresh8;

  // Set the output mode to sample/off
  init.convMode = dacConvModeSampleOff;

  // Allow the channel to be automatically refreshed
  initChannel.refreshEnable = true;

  // Initialize the DAC and DAC channel
  DAC_Init(DAC0, &init);
  DAC_InitChannel(DAC0, &initChannel, CHANNEL_NUM);

  // Enable the DAC
  DAC_Enable(DAC0, CHANNEL_NUM, true);
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
 *    Reference voltage used by the DAC
 *
 * @return
 *    The digital value that maps to the desired output voltage
 *****************************************************************************/
uint32_t getDacValue(float vOut, float vRef)
{
  return (uint32_t)((vOut * 4095) / vRef);
}

/**************************************************************************//**
 * @brief
 *    Output 0.5 volts to VDAC channel 0
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialization
  initDac();

  // Calculate the 12-bit output value for 0.5 V
  uint32_t dacValue = getDacValue(0.5, 1.25);

  // Write the output value to DAC DATA register
  DAC_ChannelOutputSet(DAC0, CHANNEL_NUM, dacValue);

  while (1) {
    EMU_EnterEM3(false); // Enter EM3 while the DAC is doing conversions
  }
}

