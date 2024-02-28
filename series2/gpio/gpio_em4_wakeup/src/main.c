/***************************************************************************//**
 * @file main.c
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

#include "mx25flash_spi.h"

/**************************************************************************//**
 * A JEDEC standard SPI flash boots up in standby mode in order to
 * provide immediate access, such as when used it as a boot memory.
 *
 * Typical current draw in standby mode for the MX25R8035F device used
 * on EFR32 radio boards is 5 µA.
 *
 * JEDEC standard SPI flash memories have a lower current deep power-down
 * mode, which can be entered after sending the relevant commands.  This
 * is on the order of 0.007 µA for the MX25R8035F.
 *****************************************************************************/
void powerDownSpiFlash(void)
{
  FlashStatus status;

  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();
}

/**************************************************************************//**
 * @brief  Initialize GPIOs for push button and LED
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);
  
  // Configure Button PB0 as input for the escape hatch
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);
  
  // Configure Button PB1 as input and EM4 wake-up source
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);
  
  // Enable GPIO pin wake-up from EM4; PB1 is EM4WUEN pin 4
  GPIO_EM4EnablePinWakeup(GPIO_IEN_EM4WUIEN4, 0);

  // Configure LED0 and LED1 outputs
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief Toggle LEDs on STK and Radio board indefinitely
 *****************************************************************************/
void toggleLEDs(void)
{
  while(1)
  {
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

    // Arbitrary delay between toggles
    for(volatile uint32_t delay = 0; delay < 0xFFFFF; delay++);
  }
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;

  CHIP_Init();

  // Turn on DCDC regulator
  EMU_DCDCInit(&dcdcInit);

  // Release pin state
  EMU_UnlatchPinRetention();

  //Initialization
  initGPIO();
  
  /**********************************************************************//**
   * When developing/debugging code on devices that enters EM2 or lower,
   * it's a good idea to have an "escape hatch" type mechanism, e.g. a
   * way to pause the device so that a debugger can connect in order
   * to erase flash, among other things.
   *
   * Before proceeding with this example, make sure PB0 is not pressed.
   * If the PB0 pin is low, turn on LED1 and execute the breakpoint
   * instruction to stop the processor in EM0 and allow a debug
   * connection to be made.
   *************************************************************************/
  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0)
  {
    GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else
  {
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);
  }

  // Get and then clear the last reset cause
  uint32_t rstCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  // If exit from EM4 caused the last reset, toggle LEDs
  if(rstCause & EMU_RSTCAUSE_EM4)
  {
    toggleLEDs();
  }
  // Otherwise, enter EM4
  else
  {
    // Power-down the radio board SPI flash
    powerDownSpiFlash();

    // Switch from DCDC regulation mode to bypass mode
    EMU_DCDCModeSet(emuDcdcMode_Bypass);

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
