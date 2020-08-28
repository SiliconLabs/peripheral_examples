/***************************************************************************//**
 * @file main_gpio_periph_s1.c
 * @brief GPIO Demo Application for Series 1
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "bsp.h"


#if defined(BSP_STK_BRD2204A) || defined(BSP_STK_BRD2102A) /* GG11 STK LFRCO out on PA12 */
#define SQUARE_WAVE_PORT gpioPortA
#define SQUARE_WAVE_PIN  12
#define SQUARE_WAVE_CLKOUTSEL CMU_CTRL_CLKOUTSEL0_LFRCO
#define SQUARE_WAVE_ROUTE_PEN CMU_ROUTEPEN_CLKOUT0PEN
#define SQUARE_WAVE_LOC CMU_ROUTELOC0_CLKOUT0LOC_LOC5
#else /* All other STKs, LFRCO out on PA0 */
#define SQUARE_WAVE_PORT gpioPortA
#define SQUARE_WAVE_PIN  0
#define SQUARE_WAVE_CLKOUTSEL CMU_CTRL_CLKOUTSEL1_LFRCO
#define SQUARE_WAVE_ROUTE_PEN CMU_ROUTEPEN_CLKOUT1PEN
#define SQUARE_WAVE_LOC CMU_ROUTELOC0_CLKOUT1LOC_LOC0
#endif

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

  /* Set PA0/PA12 as output so it can be
  overriden by the peripheral, in this case the CMU */  
  GPIO_PinModeSet(SQUARE_WAVE_PORT, SQUARE_WAVE_PIN, gpioModePushPull, 0);

  /* Enable Low Frequency RC Oscillator (LFRCO) and 
  wait until it is stable*/
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

  /* Select Clock Output 1 as Low Frequency RC(32.768 KHz) */
  CMU->CTRL = CMU->CTRL | SQUARE_WAVE_CLKOUTSEL;

  /* Route the clock output to location 2 and enable it */
  CMU->ROUTEPEN = SQUARE_WAVE_ROUTE_PEN;
  CMU->ROUTELOC0 = SQUARE_WAVE_LOC;

  while(1);
}
