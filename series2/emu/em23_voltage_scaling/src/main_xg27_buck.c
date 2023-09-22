/***************************************************************************/
/**
 * @file main_xg27_buck.c
 * @brief Example using different voltage scaling levels to show influence on
 * current draw in the Profiler
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

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_burtc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "mx25flash_spi.h"

// Included to define EMU_DCDCINIT_WSTK_DEFAULT initializer
#include "bsp.h"

// Number of 1 kHz ULFRCO clocks between BURTC interrupts
#define BURTC_IRQ_PERIOD 4000

/*
 * A JEDEC standard SPI flash boots up in standby mode in order to
 * provide immediate access, such as when used it as a boot memory.
 *
 * Typical current draw in standby mode for the MX25R8035F device used
 * on EFR32 radio boards is 5 �A, which makes  observing the difference
 * between VS2 and VS0 voltage scaling levels difficult to observe.
 *
 * Fortunately, JEDEC standard SPI flash memories have a lower current
 * deep power-down mode, which can be entered after sending the
 * relevant commands.  This is on the order of 0.007 �A for the
 * MX25R8035F, thus making the difference between between the VS2 and
 * VS0 voltage scaling modes in EM2/3 more obvious.
 */
void powerDownSpiFlash(void)
{
  FlashStatus status;
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();
}

void initBURTC()
{
  // Select ULFRCO as the BURTC clock source.
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);

  // Setup BURTC.
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.compare0Top = true;   // Reset counter at compare value
  burtcInit.start = false;        // Don't start counting yet
  CMU_ClockEnable(cmuClock_BURTC, true);
  BURTC_Init(&burtcInit);

  // Interrupt on compare match
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);

  // Set the compare value and start the counter.
  BURTC_CompareSet(0, BURTC_IRQ_PERIOD);
  BURTC_Start();
}

/**************************************************************************/
/**
 * @brief  BURTC Handler
 *****************************************************************************/
void BURTC_IRQHandler(void)
{
  uint32_t vscale;
  EMU_EM23Init_TypeDef vsInit = EMU_EM23INIT_DEFAULT;

  // Get the current EM2/3 voltage scaling
  vscale = EMU->CTRL & _EMU_CTRL_EM23VSCALE_MASK;
  if (vscale == EMU_CTRL_EM23VSCALE_VSCALE2) {
    // Currently running at VS2 (high performance), so scale down
    vsInit.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  } else {
    // Currently running at VS0 (low power), so scale up
    vsInit.vScaleEM23Voltage = emuVScaleEM23_FastWakeup;
  }

  // Switch to the new EM2/3 voltage scaling setting
  EMU_EM23Init(&vsInit);
  BURTC_IntClear(BURTC_IF_COMP);
}

/**************************************************************************/
/**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Enable DC-DC converter
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_WSTK_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Power-down the radio board SPI flash
  powerDownSpiFlash();

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
  initBURTC();

  // Power down all RAM blocks except block 0
  EMU_RamPowerDown(SRAM_BASE, 0);
  while(1){
    EMU_EnterEM3(false);
  }
}
