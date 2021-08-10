/***************************************************************************//**
 * @file main.c
 * @brief This example toggles between the VS2 and VS1 voltage scaling levels in
 * EM0. When observed using Simplicity Studio's Energy Profiler, the power
 * savings achieved by running at a DECOUPLE voltage of 1.1V (VS2) vs. 1.0V 
 * (VS1) can be observed.
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

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"

/*
 * Used to run the Fibonacci code from RAM, which makes the power
 * savings from voltage scaling more obvious in Energy Profiler.
 */
#include "em_ramfunc.h"

/**************************************************************************//**
 * For portability among radio boards, use GPIO_ODD_IRQn if
 * BSP_GPIO_PB0_PIN is odd, otherwise use GPIO_EVEN_IRQn.
 *****************************************************************************/
#if (BSP_GPIO_PB0_PIN & 1)
#define BUTTON_GPIO_IRQ GPIO_ODD_IRQn
#else
#define BUTTON_GPIO_IRQ GPIO_EVEN_IRQn
#endif

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB0 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Enable rising-edge interrupts on the PB0 pin
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, 1, 0, true);
  NVIC_ClearPendingIRQ(BUTTON_GPIO_IRQ );
  NVIC_EnableIRQ(BUTTON_GPIO_IRQ );
}

/**************************************************************************//**
 * @brief GPIO IRQ handler
 *
 * For portability among radio boards, compile GPIO_ODD_IRQHandler if
 * BSP_GPIO_PB0_PIN is odd, otherwise use GPIO_EVEN_IRQHandler.
 *****************************************************************************/
#if (BSP_GPIO_PB0_PIN & 1)
void GPIO_ODD_IRQHandler(void)
#else
void GPIO_EVEN_IRQHandler(void)
#endif
{
  EMU_EM01Init_TypeDef vsInit = EMU_EM01INIT_DEFAULT;
  EMU_VScaleEM01_TypeDef vscale;

  // Get the current voltage scaling
  vscale = EMU_VScaleGet();

  if (vscale == emuVScaleEM01_HighPerformance)
  {
    // Currently running at VS2 (high performance), so scale down
    vsInit.vScaleEM01LowPowerVoltageEnable = true;
  }
  else
  {
    // Currently running at VS1 (low power), so scale up
    vsInit.vScaleEM01LowPowerVoltageEnable = false;
  }

  /*
   * Perform voltage scaling and set the appropriate number of flash
   * wait states.
   */
  EMU_EM01Init(&vsInit);

  // Clear the PB0 pin interrupt flag
  GPIO_IntClear(1 << BSP_GPIO_PB0_PIN);
}

/**************************************************************************//**
 * @brief  Calculates the n-th Fibonacci number recursively.
 *****************************************************************************/
SL_RAMFUNC_DEFINITION_BEGIN
uint32_t fib(uint32_t n)
{
  if (n < 2)
  {
    return 1;
  }
  return (fib(n-1) + fib(n-2));
}
SL_RAMFUNC_DEFINITION_END

/**************************************************************************//**
 * @brief  Exercise the CPU and RAM in order to observe the difference
 * in current draw when core VDD is (not) scaled.
 *****************************************************************************/
SL_RAMFUNC_DEFINITION_BEGIN
int fibLoop(void)
{
  // Infinite loop
  while(1)
  {
    volatile uint32_t temp;
    for(uint32_t i = 0; i < 0x3FF; i++)
    {
      temp = fib(i);
    }
    (void)temp;
  }
}
SL_RAMFUNC_DEFINITION_END

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Turn on DCDC regulator
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_WSTK_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  /*
   * Run at 38 MHz, the highest preset frequency band common to both
   * VS2 and VS1,  in order to see the difference in current draw
   * when not scaled vs. scaled.
   */
  // Set HFRCODPLL as system clock
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFRCODPLL);
  CMU_HFRCODPLLBandSet(cmuHFRCODPLLFreq_38M0Hz);
  initGpio();

  // Run the Fibonacci code to exercise the CPU and RAM subsystems.
  fibLoop();
}
