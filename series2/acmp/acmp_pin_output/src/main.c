/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates a simple analog comparison of push button 0
 * to the 1.25V internal VREF; if the button is pushed, this pulls the
 * input voltage to GND, and the comparator output low. The comparator output is
 * routed to a GPIO. The MCU core is in sleep mode in EM3, but the analog
 * comparator remains running.
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
#include "em_acmp.h"
#include "em_burtc.h"
#include "em_usart.h"
#include "mx25flash_spi.h"
#include "bsp.h"

#define LED0_PORT           BSP_GPIO_LED0_PORT
#define LED0_PIN            BSP_GPIO_LED0_PIN
#define ACMP_OUTPUT_PORT    gpioPortB   // alternate output port/pin
#define ACMP_OUTPUT_PIN     3           // alternate output port/pin
#define ACMP_INPUT_PORT     gpioPortB
#define ACMP_INPUT_PIN      1
#define ACMP_INPUT_PORT_PIN acmpInputPB1

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
  // Configure ACMP0 Clock
  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Initialize with default settings
  ACMP_Init_TypeDef init = ACMP_INIT_DEFAULT;
  ACMP_Init(ACMP0, &init);

  // Allocate CDODD0 to ACMP0 to be able to use the input
  GPIO->BBUSALLOC = GPIO_BBUSALLOC_BODD0_ACMP0;

  // In this example we want to compare an analog input to the 1.25 V
  // internal reference. The default settings resets the divider for
  // acmpInputVREFDIV1V25, which we can use as a 1.25 V reference.
  // Now we select the two inputs to compare. Here we compare the analog
  // input to the internal 1.25V reference. When the input is lower than
  // 1.25 V then the ACMP output is 0 and when the input is higher than
  // 1.25 V then the ACMP output is 1.
  ACMP_ChannelSet(ACMP0, acmpInputVREFDIV1V25, ACMP_INPUT_PORT_PIN);

  // To be able to probe the output we can send the ACMP output to a pin.
  // The second argument to this function is the pin location which is
  // device dependent.
  ACMP_GPIOSetup(ACMP0, LED0_PORT, LED0_PIN, true, false);

  // Wait for warm-up
  while (!(ACMP0->IF & ACMP_IF_ACMPRDY));
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  FlashStatus status;

  // Use default settings for EM23 and HFXO
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_WSTK_DEFAULT;

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
  initGPIO();
  initACMP();

  // Initialize EM2/EM3 with default parameters
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  // Init and power-down MX25 SPI flash
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  // Infinite loop
  while(1)
  {
    EMU_EnterEM3(false);
  }
}
