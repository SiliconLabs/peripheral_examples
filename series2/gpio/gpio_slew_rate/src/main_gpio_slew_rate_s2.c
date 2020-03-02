/**************************************************************************//**
 * @file
 * @brief Demonstrates setting slew rate and drive strength for a GPIO port pin
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

#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_emu.h"
#include "bsp.h"
#include "retargetserial.h"
#include <stdio.h>

#define SQUARE_WAVE_PORT gpioPortC
#define SQUARE_WAVE_PIN  0

#define OUTPUT_FREQ_Hz (1000000UL)
#define TIMER_PRESCALER 2

uint32_t slewRate = 6;

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons and PA0 output.
 *****************************************************************************/
static void gpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure Button PB0 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  /* Enable EVEN interrupt to catch button press that changes slew rate */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  /* Set PA0 to push-pull output to output 1MHz square wave */
  GPIO_PinModeSet(SQUARE_WAVE_PORT, SQUARE_WAVE_PIN, gpioModePushPull, 1);

  /* Set up GPIO to output CCO 1MHz waveform */
  GPIO->TIMERROUTE[0].ROUTEEN |= GPIO_TIMER_ROUTEEN_CC0PEN; 
  GPIO->TIMERROUTE[0].CC0ROUTE = (SQUARE_WAVE_PORT << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT) | (SQUARE_WAVE_PIN << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);
}

/**************************************************************************//**
 * @brief Timer setup to output a square wave on PA0
 *****************************************************************************/
void timerSetup(void)
{
  /* Enable Timer0 Clock */
  CMU_ClockEnable(cmuClock_TIMER0, true);

  /* Set Timer0 to toggle PA0 at 1 MHz - Select clock source at fixed 20MHz FSRCO */
  CMU_ClockSelectSet(cmuClock_EM01GRPACLK, cmuSelect_FSRCO); 
  
  /* Set compare output */
  TIMER_InitCC_TypeDef initCh = TIMER_INITCC_DEFAULT;
  initCh.cofoa = timerOutputActionToggle;
  initCh.mode = timerCCModeCompare;
  TIMER_InitCC(TIMER0, 0, &initCh);

  /* Set max value of timer to cause period to be 1MHz */
  /* Timer counts from 0 - value and  overflows and toggles the pin */
  uint32_t freq = CMU_ClockFreqGet(cmuClock_TIMER0);
  TIMER_TopSet(TIMER0, freq/TIMER_PRESCALER/OUTPUT_FREQ_Hz/TIMER_PRESCALER - 1);

  /* Init timer after setting up CC as timer stops running after CC init */
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.enable = true;
  init.prescale = (TIMER_Prescale_TypeDef) _TIMER_CFG_PRESC_DIV2;
  TIMER_Init(TIMER0, &init);
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even pins.
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Button 0 changes slewrate in increments of 1 from 0-7 */
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN))
  {
    slewRate = (slewRate + 1) % 8;
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Initialize Push Buttons and PA0 */
  gpioSetup();

  /* Use printf's to show what the current slew rate setting is */
  RETARGET_SerialInit();
  
  /* Initialize PA0 square-wave output */
  timerSetup();

  printf("Press button 0 on WSTK to change slew rate in the range 0...7]\r\n");
  
  /* Infinite loop */
  while (1)
  {
    /* Update PA0 drive strength and slew rate */
    GPIO_SlewrateSet(SQUARE_WAVE_PORT, slewRate, slewRate);

    /* Print current slew rate setting */
    printf("Slew Rate: %ld\r\n", slewRate);

    /* Enter EM1 until a push-button interrupt triggers a wake-up */
    EMU_EnterEM1();
  }
}
