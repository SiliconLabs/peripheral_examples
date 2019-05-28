/**************************************************************************//**
 * @file
 * @brief This project demonstrates a simple analog comparison of push
 * button 0 (PD2) to the 1.25V internal VREF; if the button is pushed, it sets
 * LED0 (PB1) on.
 *
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
 
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_acmp.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure push button and LED
  GPIO_PinModeSet(gpioPortD, 2, gpioModeInputPullFilter, 1);
  GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortB, 1, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief ACMP initialization
 *****************************************************************************/
void initACMP(void)
{
  // Initialize with default settings
  ACMP_Init_TypeDef init = ACMP_INIT_DEFAULT;
  ACMP_Init(ACMP0, &init);
  
  // Allocate CDEVEN0 to ACMP0 to be able to use the input
  GPIO->CDBUSALLOC = GPIO_CDBUSALLOC_CDEVEN0_ACMP0;

  // In this example we want to compare an analog input to the 1.25 V
  // internal reference. The default settings resets the divider for
  // acmpInputVREFDIV1V25, which we can use as a 1.25 V reference.
  // Now we select the two inputs to compare. Here we compare the acmpInputPD2
  // input to the internal 1.25V reference. When acmpInputPD2 is lower than
  // 1.25 V then the ACMP output is 0 and when acmpInputPD2 is is higher than
  // 1.25 V then the ACMP output is 1.
  ACMP_ChannelSet(ACMP0, acmpInputVREFDIV1V25, acmpInputPD2);

  // To be able to probe the output we can send the ACMP output to a pin.
  // The second argument to this function is the pin location which is
  // device dependent.
  ACMP_GPIOSetup(ACMP0, gpioPortD, 3, true, false);

  // Wait for warmup
  while (!(ACMP0->IF & ACMP_IF_ACMPRDY));
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

  // Infinite loop
  while(1)
  {
    // Turn off LED
    GPIO_PinOutClear(gpioPortB, 1);

    // Wait for signal to go high
    while (!(ACMP0->STATUS & _ACMP_STATUS_ACMPOUT_MASK));

    // Turn on LED
    GPIO_PinOutSet(gpioPortB, 1);

    // Wait for signal to go low
    while(ACMP0->STATUS & _ACMP_STATUS_ACMPOUT_MASK);
  }
}
