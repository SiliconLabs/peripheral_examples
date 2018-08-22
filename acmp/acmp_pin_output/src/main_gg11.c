/**************************************************************************//**
 * @file
 * @brief This project demonstrates a simple analog comparison of 2 pins,
 * it compares the voltage at PB9 to the 1.25V internal VREF, if the voltage
 * is high, it sets an output pin PB11 to logic high
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
#include "em_gpio.h"
#include "em_acmp.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void) 
{
  // Enable clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure input: PB9 (Expansion Header Pin 13)
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInputPull, 0);

  // Configure output: PB11 (Expansion Header Pin 11)
  GPIO_PinModeSet(gpioPortB, 11, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief ACMP initialization
 *****************************************************************************/
void initACMP(void)
{
  // Enable clock
  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Set ACMP initialization to the default
  ACMP_Init_TypeDef acmp0_init = ACMP_INIT_DEFAULT;

  // Delay enabling ACMP until after everything has been set
  acmp0_init.enable = false;

  // Set VB Configuration to 1.25V
  ACMP_VBConfig_TypeDef vb_config = ACMP_VBCONFIG_DEFAULT;

  // Init and set ACMP channel
  ACMP_Init(ACMP0, &acmp0_init);

  // Set PB11 to ACMP0 output
  ACMP_GPIOSetup(ACMP0, 3, true, false);

  // Configure the GPIO pins such that if PB9 is high, the output is logic high
  ACMP_ChannelSet(ACMP0, acmpInputVBDIV, acmpInputAPORT2XCH25);

  ACMP_VBSetup(ACMP0, &vb_config);

  ACMP_Enable(ACMP0);

  // Wait for warmup
  while (!(ACMP0->STATUS & _ACMP_STATUS_ACMPACT_MASK)) ;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void) 
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGPIO();
  initACMP();

  // Loop
  while(1)
  {
    EMU_EnterEM3(false);
  }
}
