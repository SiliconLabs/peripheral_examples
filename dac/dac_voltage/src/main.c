/**************************************************************************//**
 * @file
 * @brief This project uses the DAC in continuous mode to output 0.5V to a pin
 * in EM1. See readme.txt for details.
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
 
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_dac.h"
#include "bsp.h"

/**************************************************************************//**
 * @brief DAC initialization
 *****************************************************************************/
void initDAC(void)
{
  // Use default settings
  DAC_Init_TypeDef        init        = DAC_INIT_DEFAULT;
  DAC_InitChannel_TypeDef initChannel = DAC_INITCHANNEL_DEFAULT;

  // Enable the DAC clock
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Calculate the DAC clock prescale value resulting in a 1 MHz DAC clock
  init.prescale = DAC_PrescaleCalc(1000000, 0);

  // Set reference to internal 1.25V reference
  init.reference = dacRef1V25;

  // Initialize the DAC and DAC channel
  DAC_Init(DAC0, &init);
  DAC_InitChannel(DAC0, &initChannel, 0);
}

/**************************************************************************//**
 * @brief  Write DAC conversion value
 *****************************************************************************/
void DAC_WriteData(DAC_TypeDef *dac, unsigned int value, unsigned int ch)
{
  // Write data output value to the correct register
  if (!ch)
  {
    dac->CH0DATA = value;
  }
  else
  {
    dac->CH1DATA = value;
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  uint32_t dacValue;

  // Chip errata
  CHIP_Init();
  
  // Initializations
  initDAC();

  // Enable DAC channel 0. See readme.txt or Datasheet for DAC0_OUT0 pin location.
  DAC_Enable(DAC0, 0, true);

  // Calculate the 12-bit output value for 0.5 V
  dacValue = (uint32_t)((0.5 * 4096) / 1.25);

  // Write the output value to DAC DATA register
  DAC_WriteData(DAC0, dacValue, 0);

  // Enter EM1 while the DAC is doing continuous conversions.
  EMU_EnterEM1();

  while (1);
}
