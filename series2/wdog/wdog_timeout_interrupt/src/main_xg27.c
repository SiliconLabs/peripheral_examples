/***************************************************************************//**
 * @file main_xg27.c
 * @brief This project demonstrates the functionality of the watchdog timer
 * interrupt. See readme.txt for details.
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

#include <stdbool.h>
#include "em_chip.h" 
#include "em_cmu.h" 
#include "em_emu.h"
#include "em_gpio.h"
#include "em_wdog.h"
#include "bspconfig.h"

volatile uint32_t msTicks;
volatile bool wdog_overflowed;

/**************************************************************************//**
 * @brief Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while((msTicks - curTicks) < dlyTicks);
}

/**************************************************************************//**
 * @brief Setup SysTick Timer for 1 msec interrupts
 *****************************************************************************/
void initSYSTICK(void)
{
  if(SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while(1);
  }
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
}

/**************************************************************************//**
 * @brief Watchdog initialization
 *****************************************************************************/
void initWDOG(void)
{
  CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_ULFRCO);
  CMU_ClockEnable(cmuClock_WDOG0, true);

  WDOG_Init_TypeDef wdogInit = WDOG_INIT_DEFAULT;
  wdogInit.debugRun = true;
  wdogInit.resetDisable = true;
  wdogInit.perSel = wdogPeriod_2k;

  WDOGn_IntEnable(WDOG0, WDOG_IEN_TOUT);
  NVIC_EnableIRQ(WDOG0_IRQn);

  WDOGn_Init(WDOG0, &wdogInit);
}

/**************************************************************************//**
 * @brief Watchdog ISR triggers when overflow occurs
 *****************************************************************************/
void WDOG0_IRQHandler(void)
{
  uint32_t flags = WDOGn_IntGet(WDOG0);
  WDOGn_IntClear(WDOG0, flags);

  wdog_overflowed = true;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  wdog_overflowed = false;

  CHIP_Init();

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  initSYSTICK();
  initGPIO();
  initWDOG();

  while(1) {
	if(wdog_overflowed) {
	  GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
	  while(1);
	}

    while(!(GPIO_PinInGet(BSP_GPIO_PB1_PORT,BSP_GPIO_PB1_PIN)));
    WDOGn_Feed(WDOG0);
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    Delay(1000);
  }
}
