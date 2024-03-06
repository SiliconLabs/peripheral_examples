/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates GPIO output from the ACMP without
 * using interrupts or having firmware otherwise drive a pin in response
 * to input voltage changes.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_acmp.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"

#include "mx25flash_spi.h"

#include "bsp.h"

#define LED0_PORT             BSP_GPIO_LED0_PORT
#define LED0_PIN              BSP_GPIO_LED0_PIN
#define ACMP_OUTPUT_PORT      gpioPortB       // alternate output port/pin
#define ACMP_OUTPUT_PIN       3               // alternate output port/pin
#define ACMP_INPUT_PORT       gpioPortB
#define ACMP_INPUT_PIN        1
#define ACMP_INPUT_PORT_PIN   acmpInputPB1

/**************************************************************************//**
 * @brief escapeHatch()
 *
 * When developing or debugging code that enters EM2 or lower, it's a
 * good idea to have an "escape hatch" type mechanism, e.g. a way to
 * pause the device so that a debugger can connect in order to erase
 * flash, among other things.
 *
 * Before proceeding with this example, make sure PB0 is not pressed.
 * If the PB0 pin is low, turn on LED0 and execute the breakpoint (BKPT)
 * instruction to stop the processor in EM0 and allow a debug
 * connection to be made.
 *****************************************************************************/
void escapeHatch(void)
{
  // Configure PB0 pin as an input pulled high
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Check for PB0 low; if so halt the CPU
  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0) {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
    __BKPT(0);
  }
  // Pin not asserted; disable the PB0 digital input
  else {
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);
  }
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure GPIO Clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure GPIO output
  GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief ACMP initialization
 *****************************************************************************/
void initACMP(void)
{
  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Initialize with default settings
  ACMP_Init_TypeDef init = ACMP_INIT_DEFAULT;
  ACMP_Init(ACMP0, &init);

  // Allocate BODD0 to ACMP0 to be able to use the input
  GPIO->BBUSALLOC = GPIO_BBUSALLOC_BODD0_ACMP0;

  /*
   * Configure ACMP0 to compare the specified input pin against the
   * selected and divided reference (which defaults to divide-by-1 if
   * the ACMP_INIT_DEFAULT settings are not overridden).
   *
   * In this example, the internal 1.25 V reference is used undivided
   * so that when the input is lower than 1.25 V, the ACMP output is 0,
   * and when it's higher than 1.25 V, the ACMP output is 1.
   */
  ACMP_ChannelSet(ACMP0, acmpInputVREFDIV1V25, ACMP_INPUT_PORT_PIN);

  // Enable and drive non-inverted comparator on specified pin
  ACMP_GPIOSetup(ACMP0, LED0_PORT, LED0_PIN, true, false);

  // Wait for warm-up
  while (!(ACMP0->IF & ACMP_IF_ACMPRDY));
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Workarounds for any errata
  CHIP_Init();

  // Turn on GPIO clock before checking escape hatch
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Halt if PB0 held before entering EM3 to allow debug access
  escapeHatch();

  /*
   * On EFR32xG22 devices and later, the debug block is powered off by
   * default in EM2 and lower energy modes.  Setting the EM2DBGEN bit in
   * the EMU_CTRL register keeps the debug logic powered in low-energy
   * modes at the expense of higher current (around 0.5 �A).
   */
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;

  /*
   * Enable LED0 pin as an output and drive low.  Note that LED0 remains
   * off until the input falls below the comparator reference voltage.
   */
  GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 0);

  initACMP();

  // Power-down MX25 SPI flash
  FlashStatus status;

  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  // Initialize EM2/3 with default parameters
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_EM23Init(&em23Init);

  // Wait in lowest power state while the comparator runs
  while(1)
  {
    EMU_EnterEM3(false);
  }
}
