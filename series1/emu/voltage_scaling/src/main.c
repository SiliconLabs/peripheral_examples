/***************************************************************************//**
 * @file main.c
 * @brief Example using different voltage scaling levels to show influence on
 * current draw in the Profiler
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

#include "em_ramfunc.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB0 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Enable falling-edge interrupts for PB pins
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 1, 0, true);

  // Enable IRQ for even numbered GPIO pins
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  // Enable IRQ for even numbered GPIO pins
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief  Scale Voltage Down
 *****************************************************************************/
void scaleDown(CMU_HFRCOFreq_TypeDef freq)
{
  // HFCLK must be < CMU_VSCALEEM01_LOWPOWER_VOLTAGE_CLOCK_MAX (20 MHz) before scaling down
  if (freq > CMU_VSCALEEM01_LOWPOWER_VOLTAGE_CLOCK_MAX)
  {
    freq = cmuHFRCOFreq_19M0Hz;
  }

  CMU_HFRCOBandSet(freq);

  // EM0 and 1 initialization structure
  EMU_EM01Init_TypeDef vsInit;

  // Scaling down from VSCALE2 (1.2 V) to VSCALE0 (1.0 V)
  vsInit.vScaleEM01LowPowerVoltageEnable = true;

  // Flash wait state at VSCALE0 will be set during scaling voltage down
  EMU_EM01Init(&vsInit);
}

/**************************************************************************//**
 * @brief Scale Voltage Up
 *****************************************************************************/
void scaleUp(CMU_HFRCOFreq_TypeDef freq)
{
  // EM0 and 1 initialization structure
  EMU_EM01Init_TypeDef vsInit;

  // Scaling up from VSCALE0 (1.0 V) to VSCALE2 (1.2 V)
  vsInit.vScaleEM01LowPowerVoltageEnable = false;

  // Flash wait state at VSCALE2 will be restored during scaling voltage up
  EMU_EM01Init(&vsInit);

  // HFCLK can now be restored after scaling up
  CMU_HFRCOBandSet(freq);
}

/**************************************************************************//**
 * @brief Push Button handler
 *****************************************************************************/
void GPIO_Handler(void)
{
  EMU_VScaleEM01_TypeDef vscale;

  vscale = EMU_VScaleGet();

  if (vscale == emuVScaleEM01_HighPerformance)
  {
    scaleDown(cmuHFRCOFreq_19M0Hz);
  }
  else
  {
    scaleUp(cmuHFRCOFreq_19M0Hz);
  }

  // Clear all even pin interrupt flags
  GPIO_IntClear(1 << BSP_GPIO_PB0_PIN);
}

/**************************************************************************//**
 * @brief GPIO Odd IRQ redirects to generic handler
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Handler();
}

/**************************************************************************//**
 * @brief GPIO Even IRQ redirects to generic handler
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Handler();
}

/**************************************************************************//**
 * @brief  Calculates the n-th fibonacci number recursively
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
 * @brief  Runs the processor for profiling purposes
 *****************************************************************************/
SL_RAMFUNC_DEFINITION_BEGIN
int fibLoop(void)
{
  // Infinite loop
  while(1)
  {
    volatile uint32_t temp;

    for(uint32_t i=0; i<0xFFF; i++)
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

  // Configure PB0
  initGpio();

  // Calculate fibonacci numbers to work-out CPU
  fibLoop();
}
