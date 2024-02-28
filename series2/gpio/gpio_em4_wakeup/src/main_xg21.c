/***************************************************************************//**
 * @file main_xg21.c
 * @brief This project demonstrates the ability for a pin to wake the device
 * from EM4.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_gpio.h"
#include "em_emu.h"
#include "em_rmu.h"

#include "bsp.h"

#define EM4WU_EM4WUEN_NUM   (9)                       // PD2 is EM4WUEN pin 9
#define EM4WU_EM4WUEN_MASK  (1 << EM4WU_EM4WUEN_NUM)

/**************************************************************************//**
 * @brief  Initialize GPIOs for push button and LED
 *****************************************************************************/
void initGPIO(void)
{
  // Configure button PB0 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Enable GPIO pin wake-up from EM4; PD2 (button 0) is EM4WU pin 9
  GPIO_EM4EnablePinWakeup(EM4WU_EM4WUEN_MASK << _GPIO_EM4WUEN_EM4WUEN_SHIFT, 0);

  // Configure Button PB1 as input for the escape hatch
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 1);

  // Configure LED0 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief Toggle LED0 on mainboard indefinitely
 *****************************************************************************/
void toggleLED(void)
{
  while(1)
  {
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

    // Arbitrary delay between toggles
	for(volatile uint32_t delay = 0; delay < 0xFFFFF; delay++);
  }
}

/**************************************************************************//**
 * @brief	Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Release pin state when coming out of EM4
  EMU_UnlatchPinRetention();

  // Initialization
  initGPIO();

  /**********************************************************************//**
   * When developing/debugging code on devices that enters EM2 or lower,
   * it's a good idea to have an "escape hatch" type mechanism, e.g. a
   * way to pause the device so that a debugger can connect in order
   * to erase flash, among other things.
   *
   * Before proceeding with this example, make sure PB1 is not pressed.
   * If the PB1 pin is low, turn on LED1 and execute the breakpoint
   * instruction to stop the processor in EM0 and allow a debug
   * connection to be made.
   *************************************************************************/
  if (GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN) == 0)
  {
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else
  {
    GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeDisabled, 0);
  }

  // Get and then clear the last reset cause
  uint32_t rstCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  // If exit from EM4 caused the last reset, toggle LEDs
  if(rstCause & EMU_RSTCAUSE_EM4)
  {
    toggleLED();
  }
  // Otherwise, enter EM4
  else
  {
    // Initialize EM4 with pin retention through wake-up
    EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
    em4Init.pinRetentionMode = emuPinRetentionLatch;

    EMU_EM4Init(&em4Init);

    // Enter EM4
    EMU_EnterEM4();
  }

  // This line should never be reached
  while(1);
}
