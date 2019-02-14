/**************************************************************************//**
 * @file vmon_interrupt.c
 * @brief VMON Interrupt Example
 * @author Silicon Labs
 * @version 1.00
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_system.h"
#include "em_gpio.h"
#include "bsp.h"

/* Change this to change voltage threshold.  Current is 3.0 volts */
#define THRESHOLD_VOLTAGE    3000

/* Change these to change vmon source */
#define VMON_CHANNEL  emuVmonChannel_AVDD
#define VMON_IF_BOTH_EDGES       EMU_IF_VMONAVDDFALL | EMU_IF_VMONAVDDRISE

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

  /* Initialize VMON */
  EMU_VmonInit(&vmonInit);

  /* Enable VMON for AVDD interrupts*/
  EMU_IntEnable(VMON_IF_BOTH_EDGES);

  /* Enable VCMP interrupts */
  NVIC_ClearPendingIRQ(EMU_IRQn);
  NVIC_EnableIRQ(EMU_IRQn);
}

/**************************************************************************//**
 * @brief  EMU Handler
 *****************************************************************************/
void EMU_IRQHandler(void)
{
  /* Check if voltage has dropped below threshold */
  bool voltageAboveMax = EMU_VmonChannelStatusGet(VMON_CHANNEL);

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
  EMU_IntClear(VMON_IF_BOTH_EDGES);
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

  /* Enter EM2 whenever not in interrupt */
  while (1)
  {
    EMU_EnterEM2(false);
  }
}
