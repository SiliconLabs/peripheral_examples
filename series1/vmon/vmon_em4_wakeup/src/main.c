/***************************************************************************//**
 * @file main.c
 * @brief VMON EM4 Wakeup Example
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

#include "em_chip.h"
#include "em_device.h"
#include "em_system.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_gpio.h"
#include "bsp.h"

/* Change this to change voltage threshold.  Current is 3.0 volts */
#define THRESHOLD_VOLTAGE   3000

/* Change these to change vmon source */
#define VMON_CHANNEL        emuVmonChannel_AVDD

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LEDs as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  // Configure PB0 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief VMON initialization
 *****************************************************************************/
void initVmon(void)
{
  /* Declare VMON_Init typedef */
  EMU_VmonInit_TypeDef vmonInit = EMU_VMONINIT_DEFAULT;

  /* Configure channel */
  vmonInit.channel = VMON_CHANNEL;

  /* Configure threshold */
  vmonInit.threshold = THRESHOLD_VOLTAGE;

  /* Wakeup from EM4 on falling edges */
  vmonInit.fallWakeup = true;

  /* Initialize VMON */
  EMU_VmonInit(&vmonInit);
}

/**************************************************************************//**
 * @brief
 *    Configure EM4 settings
 *****************************************************************************/
void initEm4(void)
{
  /* Enable clock for accessing low energy modules */
  CMU_ClockEnable(cmuClock_HFLE, true);

  /* Declare EM4_Init typedef */
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

  /* Configure for Hibernate */
  em4Init.em4State = emuEM4Hibernate;

  /* Initialize EM4 */
  EMU_EM4Init(&em4Init);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main()
{
  /* Initialize chip */
  CHIP_Init();

  /* Initialize GPIO */
  initGpio();

  /* Initialize VCMP */
  initVmon();

  /* Initialize EM4 */
  initEm4();

  /* Get the last Reset Cause */
  uint32_t rstCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  /* If the last Reset was due to leaving EM4, turn on LED. Else, enter EM4 */
  if (rstCause == RMU_RSTCAUSE_EM4RST)
  {
    GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    EMU_EnterEM2(false);
  }
  else
  {
     //push BTN0 to avoid to enter EM4
    if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0)
    {
      while(1);
    }
    EMU_EnterEM4H();
  }

  /* This should be unreachable */
  while (1);
}
