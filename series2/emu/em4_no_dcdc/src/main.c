/***************************************************************************/
/**
 * @file main.c
 * @brief  Example that demonstrates datasheet current consumption configuration
 * and current consumption numbers in EM4 Energy Mode
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

#include <stdint.h>
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_gpio.h"
#include "em_burtc.h"
#include "mx25flash_spi.h"
#include "bspconfig.h"
#define BURTC_LFXO_EN (0)

#ifdef _SILICON_LABS_32B_SERIES_2_CONFIG_7
#define LED0_OUT 0
#else
#define LED0_OUT 1
#endif

/***************************************************************************//**
 * @brief
 *   Enter EM4 with BURTC running on a LFXO
 *
 * @details
 *   Parameter:
 *     EM4H. Hibernate Mode.@n
 *   Condition:
 *     BURTC, 128 byte RAM, 32.768 kHz LFXO.@n
 *
 ******************************************************************************/
void em_EM4_LfxoBURTC(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = BSP_LFXOINIT_DEFAULT;
  CMU_LFXOInit(&lfxoInit);

  // Select LFXO as the BURTC clock source.
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFXO);

  // Setup BURTC.
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  CMU_ClockEnable(cmuClock_BURTC, true);
  BURTC_Init(&burtcInit);

  // Enter EM4
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.retainLfxo = true;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}

/***************************************************************************//**
 * @brief
 *   Enter EM4 without BURTC.
 *
 * @details
 *   Parameter:
 *     EM4.@n
 *   Condition:
 *     No BURTC, 128 byte BURAM.@n
 *
 ******************************************************************************/
void em_EM4(void)
{
  // Enter EM4
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}

/**************************************************************************/
/**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  FlashStatus status;
  uint32_t cause;

  // Chip errata
  CHIP_Init();
  EMU_UnlatchPinRetention();
  cause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();
  while ((cause & EMU_RSTCAUSE_EM4)) {
  }

  /*
   * When developing or debugging code that enters EM2 or
   *  lower, it's a good idea to have an "escape hatch" type
   * mechanism, e.g. a way to pause the device so that a debugger can
   * connect in order to erase flash, among other things.
   *
   * Before proceeding with this example, make sure PB0 is not pressed.
   * If the PB0 pin is low, turn on LED0 and execute the breakpoint
   * instruction to stop the processor in EM0 and allow a debug
   * connection to be made.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);
  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0) {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, LED0_OUT);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else {
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);
    CMU_ClockEnable(cmuClock_GPIO, false);
  }

  // Init and power-down MX25 SPI flash
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  // Enter EM4
  if(BURTC_LFXO_EN){
      em_EM4_LfxoBURTC();
  }
  else{
      em_EM4();
  }
}
