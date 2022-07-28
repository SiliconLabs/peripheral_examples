/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates a simple analog comparison of push button 0
 * to the 1.25V internal VREF; if the button is pushed, it sets LED0 on.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_cmu.h"
#include "bsp.h"

#define LED0_PORT           BSP_GPIO_LED0_PORT
#define LED0_PIN            BSP_GPIO_LED0_PIN
#define ACMP_OUTPUT_PORT    gpioPortB
#define ACMP_OUTPUT_PIN     3
#define ACMP_INPUT_PORT     gpioPortB
#define ACMP_INPUT_PIN      1
#define ACMP_INPUT_PORT_PIN acmpInputPB1

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure GPIO Clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LED and ACMP GPIO output pins
  GPIO_PinModeSet(ACMP_OUTPUT_PORT, ACMP_OUTPUT_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief ACMP initialization
 *****************************************************************************/
void initACMP(void)
{
  // Configure ACMP Clock
  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Initialize with default settings
  ACMP_Init_TypeDef init = ACMP_INIT_DEFAULT;
  ACMP_Init(ACMP0, &init);
  
  // Allocate CDEVEN0 to ACMP0 to be able to use the input
  GPIO->BBUSALLOC = GPIO_BBUSALLOC_BODD0_ACMP0;

  // In this example we want to compare an analog input to the 1.25 V
  // internal reference. The default settings resets the divider for
  // acmpInputVREFDIV1V25, which we can use as a 1.25 V reference.
  // Now we select the two inputs to compare. Here we compare the input pin
  // to the internal 1.25V reference. When the input pin is lower than
  // 1.25 V then the ACMP output is 0 and when the input pin is higher than
  // 1.25 V then the ACMP output is 1.
  ACMP_ChannelSet(ACMP0, acmpInputVREFDIV1V25, ACMP_INPUT_PORT_PIN);

  // To be able to probe the output we can send the ACMP output to a pin.
  // The second argument to this function is the pin location which is
  // device dependent.
  ACMP_GPIOSetup(ACMP0, ACMP_OUTPUT_PORT, ACMP_OUTPUT_PIN, true, false);

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
    GPIO_PinOutClear(LED0_PORT, LED0_PIN);

    // Wait for signal to go high
    while (!(ACMP0->STATUS & _ACMP_STATUS_ACMPOUT_MASK));

    // Turn on LED
    GPIO_PinOutSet(LED0_PORT, LED0_PIN);

    // Wait for signal to go low
    while(ACMP0->STATUS & _ACMP_STATUS_ACMPOUT_MASK);
  }
}
