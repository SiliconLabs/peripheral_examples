/**************************************************************************//**
 * @file
 * @brief Demonstrates outputting clock to a GPIO
 * @version 0.0.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2020 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
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
