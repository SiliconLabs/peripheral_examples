/**************************************************************************//**
 * @file
 * @brief This project uses the VDAC in continuous mode to output 0.5V to a pin
 * in EM1. See readme.txt for details.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
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
#include "em_vdac.h"
#include "bsp.h"

/**************************************************************************//**
 * @brief VDAC initialization
 *****************************************************************************/
void initVDAC(void)
{
  // Use default settings
  VDAC_Init_TypeDef        init        = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

  // Enable the VDAC clock
  CMU_ClockEnable(cmuClock_VDAC0, true);

  // VDAC clock source is 12 MHz internal VDAC oscillator
  init.asyncClockMode = true;

  // Calculate the VDAC clock prescaler value resulting in a 1 MHz VDAC clock
  init.prescaler = VDAC_PrescaleCalc(1000000, false, 0);

  // Set reference to internal 1.25V low noise reference
  init.reference = vdacRef1V25Ln;

  // Initialize the VDAC and VDAC channel
  VDAC_Init(VDAC0, &init);
  VDAC_InitChannel(VDAC0, &initChannel, 0);
}

/**************************************************************************//**
 * @brief  Write VDAC conversion value
 *****************************************************************************/
void VDAC_WriteData(VDAC_TypeDef *vdac, unsigned int value, unsigned int ch)
{
  // Write data output value to the correct register
  if (!ch)
  {
    vdac->CH0DATA = value;
  }
  else
  {
    vdac->CH1DATA = value;
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  uint32_t vdacValue;

  // Chip errata
  CHIP_Init();
  
  // Initializations
  initVDAC();

  // Enable DAC channel 0. See readme.txt or Datasheet for VDAC0_OUT0 pin location.
  VDAC_Enable(VDAC0, 0, true);

  // Calculate the 12-bit output value for 0.5 V
  vdacValue = (uint32_t)((0.5 * 4096) / 1.25);

  // Write the output value to VDAC DATA register
  VDAC_WriteData(VDAC0, vdacValue, 0);

  // Enter EM1 while the VDAC is doing continuous conversions.
  EMU_EnterEM1();

  while (1);
}
