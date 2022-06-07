/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the functionality of the watchdog timer. It
 * uses LED0 and a Push-button to indicate the state of the system. If the LED
 * is flashing, the system has not undergone a reset. If the Push Button 0 is
 * pressed and held for some time, it will trigger a WDOG reset. See readme.txt
 * for details.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_rmu.h"
#include "em_system.h"
#include "em_wdog.h"
#include "bspconfig.h"

// GLOBAL VARIABLES 
unsigned long resetCause;
volatile uint32_t msTicks; // counts 1ms timeTicks 

// Function Declarations 
void initGPIO(void);
void initWDOG(void);

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       // increment counter necessary in Delay()
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata 
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  #if defined( _EMU_DCDCCTRL_MASK )
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  #endif

  // Store the cause of the last reset, and clear the reset cause register 
  resetCause = RMU_ResetCauseGet();
  // Clear Reset causes so we know which reset occurs the next time 
  RMU_ResetCauseClear();

  // Configure the Push Buttons and the LEDs 
  initGPIO();

  // Setup SysTick Timer for 1 msec interrupts  
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

  // Configure and Initialize the Watchdog timer 
  initWDOG();

  // Check if Watch Dog (WDOG0) triggered the last reset
  if (resetCause & EMU_RSTCAUSE_WDOG0)
  {
    // Turn LED0 on
    GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    while(1); //Stay here
  }

  // Enter loop, and feed the WDOG 
  while (1)
  {
    // Do not feed the WDOG if PB0 is pressed 
    while(!(GPIO_PinInGet(BSP_GPIO_PB0_PORT,BSP_GPIO_PB0_PIN)));

    // Feed the watchdog
    WDOGn_Feed(DEFAULT_WDOG);

    // Toggle LED0 at 10 kHz
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    Delay(100);
  }
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable clock for the GPIO module; has no effect on xG21
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LED0 
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  // Configure PB0 as input  
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
}

/**************************************************************************//**
 * @brief Watchdog initialization
 *****************************************************************************/
void initWDOG(void)
{
  // Enable clock for the WDOG module; has no effect on xG21
  CMU_ClockEnable(cmuClock_WDOG0, true);

  // Watchdog Initialize settings 
  WDOG_Init_TypeDef wdogInit = WDOG_INIT_DEFAULT;
  CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_ULFRCO); /* ULFRCO as clock source */
  wdogInit.debugRun = true;
  wdogInit.em3Run = true;
  wdogInit.perSel = wdogPeriod_2k; // 2049 clock cycles of a 1kHz clock  ~2 seconds period

  // Initializing watchdog with chosen settings 
  WDOGn_Init(DEFAULT_WDOG, &wdogInit);
}
