/**************************************************************************//**
 * @file
 * @brief This project demonstrates a simple analog comparison of PB9
 * to the 1.25V internal VREF; when the voltage is below 1.25, LED0 is red.
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
#include "em_acmp.h"
#include "em_gpio.h"
#include "bsp.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure input: PB9 (Expansion Header Pin 13)
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInputPull, 0);
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

  // We want to delay enable until after everything is set up
  acmp0_init.enable = false;

  // Set VB to default configuration of 1.25V
  ACMP_VBConfig_TypeDef vb_config = ACMP_VBCONFIG_DEFAULT;

  // Init and set ACMP channel
  ACMP_Init(ACMP0, &acmp0_init);

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
  initGpio();
  initACMP();

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
