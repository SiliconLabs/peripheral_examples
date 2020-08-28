/***************************************************************************//**
 * @file main_gpio_periph_s2.c
 * @brief Demonstrates outputting clock to a GPIO
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
#include "em_gpio.h"
#include "em_chip.h"

#define  CMUCLOCKOUT_PORT  gpioPortC
#define  CMUCLOCKOUT_PIN   3
#define  SLEW_RATE         7

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Initialize chip */
  CHIP_Init();
  
  /* Enable clock for GPIO module */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Set chosen port pin as output */  
  GPIO_PinModeSet(CMUCLOCKOUT_PORT, CMUCLOCKOUT_PIN, gpioModePushPull, 0);
  
  /* Set slew rate / drive strength so there is no ringing */
  GPIO_SlewrateSet(CMUCLOCKOUT_PORT, SLEW_RATE, SLEW_RATE);

  /* Enable Low Frequency RC Oscillator (LFRCO) and 
  wait until it is stable*/
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
  
  /* Select Clock Output 1 as Low Frequency RC(32.768 KHz) */
  CMU->EXPORTCLKCTRL = CMU->EXPORTCLKCTRL | CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFRCO;
  
  /* Route the clock output to the GPIO port and pin and enable */
  GPIO->CMUROUTE.ROUTEEN |= GPIO_CMU_ROUTEEN_CLKOUT1PEN; 
  GPIO->CMUROUTE.CLKOUT1ROUTE = (CMUCLOCKOUT_PORT << _GPIO_CMU_CLKOUT1ROUTE_PORT_SHIFT) | (CMUCLOCKOUT_PIN << _GPIO_CMU_CLKOUT1ROUTE_PIN_SHIFT);

  while(1);
}
