/**************************************************************************//**
 * @file
 * @brief This example toggles between the VS2 and VS1 voltage scaling
 * levels in EM0.  When observed using Simplicity Studio's Energy
 * Profiler, the power savings achieved by running at a DECOUPLE
 * voltage of 1.1V (VS2) vs. 1.0V (VS1) can be observed.
 *
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2019 Silicon Labs, http://www.silabs.com</b>
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

  /*
   * Run at 38 MHz, the highest preset frequency band common to both
   * VS2 and VS1,  in order to see the difference in current draw
   * when not scaled vs. scaled.
   */
  CMU_HFRCODPLLBandSet(cmuHFRCODPLLFreq_38M0Hz);

  initGpio();

  // Run the Fibonacci code to exercise the CPU and RAM subsystems.
  fibLoop();
}
