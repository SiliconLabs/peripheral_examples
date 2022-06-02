/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates pulse width modulation using the LETIMER
 * module. Expansion Header Pin 5 is configured for PWM output.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_letimer.h"
#include "bsp.h"

// Desired frequency in Hz
#define OUT_FREQ 1

// Duty cycle percentage
#define DUTY_CYCLE 30

// LET0_O0 port/pin defs (Only ports A and B are available for LET0_O0 output)
// PA0 = (Expansion Header 5 (4263B/C, 4186A), Expansion Header 11 (4263A))
#define LET0OUT0PORT	gpioPortA
#define LET0OUT0PIN		0

/**************************************************************************//**
 * @brief escapeHatch()
 * When developing or debugging code that enters EM2 or
 * lower, it's a good idea to have an "escape hatch" type
 * mechanism, e.g. a way to pause the device so that a debugger can
 * connect in order to erase flash, among other things.
 *
 * Before proceeding with this example, make sure PB0 is not pressed.
 * If the PB0 pin is low, turn on LED0 and execute the breakpoint
 * instruction to stop the processor in EM0 and allow a debug
 * connection to be made.
 *****************************************************************************/
void escapeHatch(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);
  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0) {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else {
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);
    CMU_ClockEnable(cmuClock_GPIO, false);
  }
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure LET0_O0 output
  GPIO_PinModeSet(LET0OUT0PORT, LET0OUT0PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCmu(void)
{
  // Enable clock to GPIO and LETIMER0
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_LETIMER0, true);
}

/**************************************************************************//**
 * @brief Clock initialization
 *****************************************************************************/
void initClock(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

  // Select LFXO for the LETIMER
  CMU_LFXOInit(&lfxoInit);
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
}

/**************************************************************************//**
 * @brief LETIMER initialization
 *****************************************************************************/
void initLetimer(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Calculate the top value (frequency) based on clock source
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / OUT_FREQ;

  // Reload top on underflow, PWM output, and run in free mode
  letimerInit.comp0Top = true;
  letimerInit.topValue = topValue;
  letimerInit.ufoa0 = letimerUFOAPwm;
  letimerInit.repMode = letimerRepeatFree;

  // Enable LETIMER0 output0 on PA6
  GPIO->LETIMERROUTE.ROUTEEN = GPIO_LETIMER_ROUTEEN_OUT0PEN;
  GPIO->LETIMERROUTE.OUT0ROUTE = \
      (LET0OUT0PORT << _GPIO_LETIMER_OUT0ROUTE_PORT_SHIFT) \
      | (LET0OUT0PIN << _GPIO_LETIMER_OUT0ROUTE_PIN_SHIFT);

  // Set COMP0 to control duty cycle
  LETIMER_CompareSet(LETIMER0, 0, topValue * DUTY_CYCLE / 100);

  // Initialize LETIMER
  LETIMER_Init(LETIMER0, &letimerInit);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Recommended recovery procedure for code in development
  escapeHatch();

  /* Note: On EFR32xG22 devices and later, the DEBUG block on the device is 
     powered off by default in low power modes EM2 and below.  Setting the 
     EM2DBGEN bit in EMU_CTRL will cause the device to keep DEBUG powered on in
     EM2.  Because DEBUG and LETIMER are on different power sub-domains on xG23
     and this example goes into EM2 in a while(1) loop, this is necessary in
     order to reconnect the debugger for subsequent device erase and
     programming.  When the EM2DBGEN bit is set, the device will exhibit
     slightly higher EM2 current consumption than when EM2DBGEN is not set. */
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;

  // Initializations
  initCmu();
  initClock();
  initGpio();
  initLetimer();

  // Enter low energy state, PWM will continue
  while (1)
  {
    EMU_EnterEM2(true);
  }
}
