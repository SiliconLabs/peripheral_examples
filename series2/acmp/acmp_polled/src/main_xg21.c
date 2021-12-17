/***************************************************************************//**
 * @file main_xg21.c
 * @brief This project demonstrates a simple analog comparison of push button 0
 * (PD2) to the 1.25V internal VREF; if the button is pushed, it sets LED0 (PB1)
 * on.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
  // 1.25 V then the ACMP output is 0 and when acmpInputPD2 is higher than
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
