/*****************************************************************************
 * @file main_gpio_conf.c
 * @brief GPIO Demo Application
 * @author Silicon Labs
 * @version x.xx
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "bspconfig.h"

/**************************************************************************//**
 * @brief  Main function
 * Main is called from __iar_program_start, see assembly startup file
 *****************************************************************************/
int main(void)
{
  /* Initialize chip */
  CHIP_Init();
  
  /* Enable clock for GPIO module */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB0 pin as an input for PB0 button */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);

  /* Configure LED0 as a push pull for LED drive */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPullDrive, 1);

  while (1)
  {
    /* PB0 button is pulled-up, so pressing it will result 
     in reading 0 */
    if (!GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN))
    {
      /* Configure LED Port with alternate drive strength of 0.5mA */
      GPIO_DriveModeSet(BSP_GPIO_LED0_PORT, gpioDriveModeLowest);
    }
    else
    {
      /* Configure LED Port with standard drive strength of 6mA */
      GPIO_DriveModeSet(BSP_GPIO_LED0_PORT, gpioDriveModeStandard);
    }
  }
}
