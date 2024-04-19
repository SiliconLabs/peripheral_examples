/***************************************************************************//**
 * @file main_xg27.c
 * @brief This project demonstrates the prs clear functionality of the watchdog
 * timer. See readme.txt for details.
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
#include "em_prs.h"
#include "em_rmu.h"
#include "em_wdog.h"
#include "bspconfig.h"

#define GPIO_PRS_INPUT_PORT gpioPortA
#define GPIO_PRS_INPUT_PIN  5

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);

  GPIO_PinModeSet(GPIO_PRS_INPUT_PORT, GPIO_PRS_INPUT_PIN, gpioModeInput, 0);
  GPIO_ExtIntConfig(GPIO_PRS_INPUT_PORT, GPIO_PRS_INPUT_PIN, GPIO_PRS_INPUT_PIN, false, false, false);
}

/**************************************************************************//**
 * @brief PRS initialization
 *****************************************************************************/
void initPRS(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  PRS_SourceAsyncSignalSet(0, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO, GPIO_PRS_INPUT_PIN);
  PRS_ConnectConsumer(0, prsTypeAsync, prsConsumerWDOG0_SRC0);
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
  wdogInit.em3Run = true;
  wdogInit.clrSrc = true;
  wdogInit.perSel = wdogPeriod_2k;

  WDOGn_Init(WDOG0, &wdogInit);
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

  initGPIO();
  initPRS();
  initWDOG();

  while(1) {
	EMU_EnterEM3(false);
  }
}
