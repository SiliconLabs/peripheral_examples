/***************************************************************************//**
 * @file main.c
 * @brief VCMP Interrupt Example
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
#include "em_emu.h"
#include "em_cmu.h"
#include "em_system.h"
#include "em_vcmp.h"
#include "em_gpio.h"
#include "bsp.h"

#define MAX_VOLTAGE    3.0

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LEDs as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief VCMP initialization
 *****************************************************************************/
void initVcmp(void)
{
  /* Enable clock */
  CMU_ClockEnable(cmuClock_VCMP, true);

  /* Declare VCMP_Init typedef */
  VCMP_Init_TypeDef vcmpInit = VCMP_INIT_DEFAULT;

  /* Enable falling interrupts */
  vcmpInit.irqFalling = true;

  /* Set trigger level to max voltage */
  vcmpInit.triggerLevel = VCMP_VoltageToLevel(MAX_VOLTAGE);

  /* Initialize VCMP */
  VCMP_Init(&vcmpInit);

  /* Enable VCMP Edge interrupt */
  VCMP_IntEnable(VCMP_IEN_EDGE);

  /* Enable VCMP interrupts */
  NVIC_ClearPendingIRQ(VCMP_IRQn);
  NVIC_EnableIRQ(VCMP_IRQn);
}

/**************************************************************************//**
 * @brief  VCMP Handler
 *****************************************************************************/
void VCMP_IRQHandler(void)
{
  /* Check if voltage has dropped below threshold */
  bool voltageAboveMax = VCMP_VDDHigher();

  /* Set LED */
  if (voltageAboveMax)
  {
    GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
  }
  else
  {
    GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
  }
  /* Clear interrupt flag */
  VCMP_IntClear(VCMP_IFC_EDGE);
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
  initVcmp();

  /* Enter EM2 whenever not in interrupt */
  while (1)
  {
    EMU_EnterEM2(false);
  }
}
