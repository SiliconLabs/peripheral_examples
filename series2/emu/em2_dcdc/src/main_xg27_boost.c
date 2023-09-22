/***************************************************************************/
/**
 * @file main_xg27_boost.c
 * @brief Example that demonstrates datasheet current consumption configuration
 * and current consumption numbers in EM2 Energy Mode.
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

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_burtc.h"
#include "mx25flash_spi.h"
#include "bspconfig.h"
#define POWER_DOWN_RAM  (1)
// This define decides whether to power down MX25 SPI flash
#define MX25_POWER_DOWN (1)

/**************************************************************************/
/**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  FlashStatus status;

  // Chip errata
  CHIP_Init();

  // Turn on DCDC regulator
  EMU_DCDCBoostInit_TypeDef dcdcInit = EMU_DCDCBOOSTINIT_DEFAULT;
  EMU_DCDCBoostInit(&dcdcInit);

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
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else {
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);
    CMU_ClockEnable(cmuClock_GPIO, false);
  }

  // Enable voltage downscaling in EM mode 2(VSCALE0)
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;

  // Initialize EM23 energy modes
  EMU_EM23Init(&em23Init);

  /*
   * The BRD4111A EFR32BG27 boost radio board has specific power configuration
   * requirements in order to measure low energy mode current consumption
   * accurately(refer to the readme for configuration details). This change in
   * configuration, however, will effectively disconnect the power source to the
   * MX25 flash. Therefore, there is no need to power down the MX25 SPI flash
   * when the board is modified. This routine is only needed for an unmodified
   * radio board.
   */
  if(MX25_POWER_DOWN) {
    // Init and power-down MX25 SPI flash
    MX25_init();
    MX25_RSTEN();
    MX25_RST(&status);
    MX25_DP();
    MX25_deinit();
  }

  // Route desired oscillator to BURTC clock tree
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFRCO);

  // Setup BURTC parameters
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;

  // Initialize BURTC
  CMU_ClockEnable(cmuClock_BURTC, true);
  BURTC_Reset();
  BURTC_Init(&burtcInit);

  // Power down all RAM blocks except block 0
  if (POWER_DOWN_RAM) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Enter EM2
  EMU_EnterEM2(false);
}
