/**************************************************************************//**
 * @file
 * @brief This project demonstrates a simple analog comparison of 2 pins,
 * it compares the voltage at push button 1 to the 2.5V internal VREF,
 * if the button is pushed, it sets an output pin PA1 to logic high
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "em_acmp.h"
#include "em_emu.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void) 
{
  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);
}

void initACMP(void)
{
  // Enable clock and ACMP
  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Set ACMP initialization to the default
  ACMP_Init_TypeDef acmp0_init = ACMP_INIT_DEFAULT;

  // Init and set ACMP channel
  ACMP_Init(ACMP0, &acmp0_init);

  // Set pin modes for the input and output pins
  ACMP_GPIOSetup(ACMP0, BSP_GPIO_PB1_PORT, true, false);
  ACMP_GPIOSetup(ACMP0, gpioPortA, true, false);

  // Configure the GPIO pins such that if PB1 is pushed, the output is logic high
  ACMP_ChannelSet(ACMP0, _ACMP_INPUTSEL_POSSEL_APORT1YCH23,_ACMP_INPUTSEL_VBSEL_2V5);

  // Wait for warmup
  while (!(ACMP0->STATUS & ACMP_STATUS_ACMPACT)) ;

  // Set pin PA1(EXP 14) as the output location
  ACMP0->ROUTEPEN |= ACMP_ROUTEPEN_OUTPEN;
  ACMP0->ROUTELOC0 |= ACMP_ROUTELOC0_OUTLOC_LOC1;

  ACMP_Enable(ACMP0);

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
  }
}
