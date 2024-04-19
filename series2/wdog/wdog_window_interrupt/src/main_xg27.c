/***************************************************************************//**
 * @file main_xg27.c
 * @brief This project demonstrates the window interrupt functionality of the
 * watchdog timer. See readme.txt for details.
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
#include "em_rmu.h"
#include "em_wdog.h"
#include "bspconfig.h"

volatile uint32_t msTicks = 0;
volatile uint32_t msDelay = 1100;
volatile bool clearedTooEarly = false;

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
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
 * @brief SysTick initialization
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
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
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
  wdogInit.winSel = wdogIllegalWindowTime50_0pct;
  wdogInit.perSel = wdogPeriod_2k;

  WDOGn_IntEnable(WDOG0, WDOG_IEN_WIN);
  NVIC_EnableIRQ(WDOG0_IRQn);

  WDOGn_Init(WDOG0, &wdogInit);
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even pins.
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  uint32_t flags = GPIO_IntGet();
  GPIO_IntClear(flags);

  msDelay =  800;
}

/**************************************************************************//**
 * @brief Watchdog ISR triggers when counter reaches warning level
 *****************************************************************************/
void WDOG0_IRQHandler(void)
{
  uint32_t flags = WDOGn_IntGet(WDOG0);
  WDOGn_IntClear(WDOG0, flags);

  clearedTooEarly = true;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  uint32_t resetCause;

  CHIP_Init();

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  resetCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  if(resetCause & EMU_RSTCAUSE_WDOG0) {
	CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
    while(1);
  }

  initSYSTICK();
  initGPIO();
  initWDOG();

  while(1) {
    if(clearedTooEarly) {
      GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
      while(1);
    }
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    Delay(msDelay);
    WDOGn_Feed(WDOG0);
  }
}
