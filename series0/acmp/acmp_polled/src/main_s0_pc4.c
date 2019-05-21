/**************************************************************************//**
 * @file
 * @brief This project demonstrates a simple analog comparison of 2 pins,
 * it compares the voltage in an input pin (see README) to the 1.25V internal
 * VREF, if the voltage is high, it sets LED0 on.
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
void initGpio(void)
{
  // Enable clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PC4 for input (See README for Exp Header pin)
  GPIO_PinModeSet(gpioPortC, 4, gpioModeInputPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief ACMP initialization
 *****************************************************************************/
void initAcmp(void)
{
  // Enable clock
  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Set ACMP initialization to the default
  ACMP_Init_TypeDef acmp0_init = ACMP_INIT_DEFAULT;

  // Init and set ACMP channel
  ACMP_Init(ACMP0, &acmp0_init);

  // Configure the GPIO pins such that if input is higher than 1.25V, the output is logic high
  ACMP_ChannelSet(ACMP0, acmpChannel1V25, acmpChannel4);

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
  initGpio();
  initAcmp();

  while(1){
    // Turn off LED
    GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

    // Wait for signal to go high
    while (!(ACMP0->STATUS & _ACMP_STATUS_ACMPOUT_MASK));

    // Turn on LED
    GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

    // Wait for signal to go low
    while(ACMP0->STATUS & _ACMP_STATUS_ACMPOUT_MASK);
  }
}
