/***************************************************************************//**
 * @file main_s1.c
 * @brief This project demonstrates a simple analog comparison of 2 pins, it
 * compares the voltage at PB0 to the 1.25V internal VREF, if the button is
 * pushed, it enters the ACMP handler
 *
 * Note: Analog pin inputs cannot exceed the minimum of IOVDD or AVDD + 0.3V,
 * regardless of whether OVT is enabled or disabled.
 ******************************************************************************
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

  // Configure Push Button 0 for analog input to ACMP
  // It is recommended to set the pin mode to disabled for analog inputs.
  // See the GPIO description in the device reference manual for more details.
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);

  // Configure LED0 pin
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  // Disable OVT for pins used as analog inputs. Disabling the over-voltage
  // capability will provide less distortion on analog inputs.
  // Analog pin inputs cannot exceed the minimum of IOVDD or AVDD + 0.3V,
  // regardless of whether OVT is enabled or disabled.
  GPIO->P[BSP_GPIO_PB0_PORT].OVTDIS |= 1 << BSP_GPIO_PB0_PIN;
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

  // Configure the GPIO pins such that if PB0 is pushed, the output is logic high
  ACMP_ChannelSet(ACMP0, acmpInputAPORT2YCH22, acmpInputVBDIV);

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
