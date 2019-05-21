/**************************************************************************//**
 * @file
 * @brief This project demonstrates a simple analog comparison of 2 pins,
 * it compares the voltage at Expansion Header Pin 3 to the 1.25V internal VREF.
 * If the voltage on the expansion header is over 1.25V, an output pin will be 
 * set high (see README to details).
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
#include "bsp.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure Expansion Header Pin 3 as input
  GPIO_PinModeSet(gpioPortC, 0, gpioModeInputPull, 0);

  // Configure PE2 as output (LED0)
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
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

  // Init and set ACMP channel
  ACMP_Init(ACMP0, &acmp0_init);

  // Set PE2 (LED0) to output
  ACMP_GPIOSetup(ACMP0, 1, true, false);

  // Configure the GPIO pins such that if Exp Header 3 is higher than 1.25V, the output is logic high
  ACMP_ChannelSet(ACMP0, acmpChannel1V25, acmpChannel0);

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
