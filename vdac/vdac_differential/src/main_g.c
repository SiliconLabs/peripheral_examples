/**************************************************************************//**
 * @file main_g.c
 * @brief This project uses the DAC in continuous mode with differential output
 * to output a difference of 0.5V between two pins in EM1. See readme.txt for
 * details.
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

/**************************************************************************//**
 * @brief 
 *    DAC initialization
 *
 * @details
 *    This program uses the main DAC output for channel 0 and channel 1
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
  
  // Set the output mode to differential instead of single-ended
  init.diff = true;

  // Initialize the DAC as well as DAC channels 0 and 1
  DAC_Init(DAC0, &init);
  DAC_InitChannel(DAC0, &initChannel, 0);
  DAC_InitChannel(DAC0, &initChannel, 1);
  
  // Enable the DAC channels
  DAC_Enable(DAC0, 0, true);
  DAC_Enable(DAC0, 1, true);
}

/**************************************************************************//**
 * @brief
 *    Calculate the digital value that maps to the desired output voltage
 * 
 * @notes
 *    The vRef parameter must match the reference voltage selected during
 *    initialization.
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
  return (uint32_t)((vOut * 2047) / vRef);
}

/**************************************************************************//**
 * @brief  
 *    Continuously output 0.5 volts to DAC channel 0
 *
 * @notes
 *    The source for differential mode is channel 0
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
  int channel = 0;
  DAC_ChannelOutputSet(DAC0, channel, dacValue);

  while (1) {
    EMU_EnterEM1(); // Enter EM1 while the DAC is doing continuous conversions
  }
}

