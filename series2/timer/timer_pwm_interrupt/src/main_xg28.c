/***************************************************************************//**
 * @file main_xg28.c
 *
 * @brief This project demonstrates interrupt-driven pulse width
 * modulation using the TIMER module.  GPIO pin PB4 is configured to
 * output a 1 kHz, 30% duty cycle signal.  The duty cycle can be
 * adjusted by writing to the TIMER_CC_OCB register in the interrupt
 * handler or by changing the value of the global dutyCycle variable.
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
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"

// Desired PWM frequency and initial duty cycle
#define PWM_FREQ            1000
#define INITIAL_DUTY_CYCLE  30

// Duty cycle global variable for IRQ handler use
static volatile float dutyCycle;

/**************************************************************************//**
 * @brief
 *    Interrupt handler for TIMER0
 *
 * @note
 *    In this example, the duty cycle of the output waveform does not
 *    change unless the value of the dutyCycle global variable is
 *    modified outside the scope of this function.
 *
 *    Alternatively, other code could be inserted here to modify the
 *    duty cycle based on some other input, e.g. the voltage measured
 *    by the IADC on a given input channel.
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  uint32_t newDutyCount;

  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(TIMER0);
  TIMER_IntClear(TIMER0, flags);

  // Calculate new duty cycle count
  newDutyCount = (uint32_t)((TIMER_TopGet(TIMER0) * dutyCycle) / 100);

  // Write OCB to update the duty cycle of the next waveform period
  TIMER_CompareBufSet(TIMER0, 0, newDutyCount);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure PB4 as Push-Pull
  GPIO_PinModeSet(gpioPortB, 4, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCMU(void)
{
  // Enable the GPIO and TIMER0 bus clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  uint32_t timerFreq, topValue, dutyCount;
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  // Don't start counter on initialization
  timerInit.enable = false;

  // PWM mode sets/clears the output on compare/overflow events
  timerCCInit.mode = timerCCModePWM;

  TIMER_Init(TIMER0, &timerInit);

  // Route CC0 output to PB4
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortB << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (4 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set top value to overflow at the desired PWM_FREQ frequency
  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);
  topValue = (timerFreq / PWM_FREQ);
  TIMER_TopSet(TIMER0, topValue);

  // Set dutyCycle global variable and compare value for initial duty cycle
  dutyCycle = INITIAL_DUTY_CYCLE;
  dutyCount = (topValue * INITIAL_DUTY_CYCLE) / 100;
  TIMER_CompareSet(TIMER0, 0, dutyCount);

  // Now start the TIMER
  TIMER_Enable(TIMER0, true);

  // Enable TIMER0 compare event interrupts to update the duty cycle
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
  NVIC_EnableIRQ(TIMER0_IRQn);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  initCMU();

  initGPIO();
  initTIMER();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}
