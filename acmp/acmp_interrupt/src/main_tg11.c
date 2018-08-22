/**************************************************************************//**
 * @file
 * @brief This project demonstrates a simple analog comparison of 2 pins,
 * it compares the voltage at an expansion header pin (see README for details)
 * to the 1.25V internal VREF.  When the voltage on that pin drops below 1.25V,
 * LED0 toggles.
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

  // Configure PC0 and LED
  GPIO_PinModeSet(gpioPortC, 0, gpioModeInputPullFilter, 1);
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

  // ACMP interrupts when voltage on pos channel drops below neg channel
  acmp0_init.interruptOnFallingEdge = true;

  // Set VB to default configuration of 1.25V
  ACMP_VBConfig_TypeDef vb_config = ACMP_VBCONFIG_DEFAULT;

  // Init and set ACMP channel
  ACMP_Init(ACMP0, &acmp0_init);

  // Configure the GPIO pins such that if PC0 is above 1.25, the output is logic high
  ACMP_ChannelSet(ACMP0, acmpInputVBDIV, acmpInputAPORT0XCH0);

  ACMP_VBSetup(ACMP0, &vb_config);

  ACMP_Enable(ACMP0);

  // Wait for warmup
  while (!(ACMP0->STATUS & _ACMP_STATUS_ACMPACT_MASK)) ;

  // Clear pending ACMP interrupts
  NVIC_ClearPendingIRQ(ACMP0_IRQn);
  ACMP_IntClear(ACMP0, ACMP_IFC_EDGE);

  // Enable ACMP interrupts
  NVIC_EnableIRQ(ACMP0_IRQn);
  ACMP_IntEnable(ACMP0, ACMP_IEN_EDGE);
}

/**************************************************************************//**
 * @brief  ACMP Handler
 *****************************************************************************/
void ACMP0_IRQHandler(void)
{
  // Toggle LED0
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

  // Clear interrupt flag
  ACMP_IntClear(ACMP0, ACMP_IFC_EDGE);
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

  // Turn off LED0
  GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

  // Infinite loop
  while(1)
  {
    // Enter EM3 until next ADC interrupt
    EMU_EnterEM3(false);
  }
}
