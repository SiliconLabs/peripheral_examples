/**************************************************************************//**
 * @file
 * @brief Example using different voltage scaling levels to show influence
 * on current draw in the Profiler
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
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
