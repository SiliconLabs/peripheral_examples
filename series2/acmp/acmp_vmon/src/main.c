/***************************************************************************//**
 * @file main.c
 * @brief This project shows how the ACMP module and its VSENSE inputs
 * can be used to monitor a supply voltage without CPU intervention.
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
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "bsp.h"

#include "mx25flash_spi.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /*
   * Configure LED pins as outputs.
   *
   * Note that both LEDs remain off until the VMCU supply (combined
   * AVDD and IOVDD on the development board) falls below the scaled
   * comparator reference voltage.
   */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
}

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
 * @brief ACMP initialization
 *****************************************************************************/
void initACMP(void)
{
  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Initialize ACMP with settings for supply monitoring
  ACMP_Init_TypeDef init =
  {
    0x2,                      // Use reduced bias for lower current
    acmpInputRangeReduced,    // Input range reduced to 0 to AVDD - 0.7 V
    acmpAccuracyHigh,         // High accuracy mode to minimize wake-ups
    acmpHysteresisDisabled,
    false,                    // Output 0 when ACMP is inactive
    0x10,                     // Scaled VREFDIV for comparison against VSENSE
    true                      // Enable after initialization
  };

  ACMP_Init(ACMP0, &init);

  /*
   * For supply monitoring, the selected reference is the negative input
   * and the desired VSENSE channel is the positive input.
   *
   * When ACMP0 is used, VSENSE0 is the AVDD supply and VSENSE1 is the
   * IOVDD supply (IOVDD0 on a device that also has an IOVDD1).  In
   * either case, the 1DIV4 designation indicates that the input voltage
   * to the comparator is divided by 4.
   *
   * In this example, the 2.5 V internal reference is multiplied by
   * 16 (0x10) / 63 (0x3F) to set a comparison threshold of 0.6349 V.
   * When AVDD = 2.54 V, VENSE0 = 2.54 V / 4 = 0.635 V.
   */
  ACMP_ChannelSet(ACMP0, acmpInputVREFDIV2V5, acmpInputVSENSE01DIV4);

  // Wait for warm-up
  while (!(ACMP0->IF & ACMP_IF_ACMPRDY));

  // Clear pending ACMP interrupts
  ACMP_IntClear(ACMP0, _ACMP_IF_MASK);
  NVIC_ClearPendingIRQ(ACMP0_IRQn);

  // Enable ACMP falling and rising interrupts
  ACMP_IntEnable(ACMP0, (ACMP_IEN_FALL | ACMP_IEN_RISE));
  NVIC_EnableIRQ(ACMP0_IRQn);
}

/**************************************************************************//**
 * @brief ACMP0 interrupt handler
 *****************************************************************************/
void ACMP0_IRQHandler(void)
{
  if ((ACMP_IntGet(ACMP0) & _ACMP_IF_FALL_MASK) == ACMP_IF_FALL)
  {
    // Supply below threshold
    GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
  }
  else
  {
    // Supply above threshold
    GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
  }

  // Clear interrupt flags
  ACMP_IntClear(ACMP0, (ACMP_IF_FALL |ACMP_IF_RISE));
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/

/**************************************************************************//**
 * In this example we are comparing AVDD (VSENSE0) input to the internal 2.5V
 * reference. When AVDD falls below threshold, LED0 lights up. If AVDD is
 * above threshold, LED1 lights up.
 *
 * Note: VREFOUT = VREFIN * (VREFDIV/63). Since VSENSE0 is divided by 4 we need
 * to also divide VREF to be comparable to VSENSE0. In this example we use
 * VREFDIV = 16 (0x10) to get the reference around 0.7V. The threshold can
 * be calculated with VREFDIV * (16/63) * 4 = 2.54V.
 *****************************************************************************/
int main(void)
{
  // Workarounds for any errata
  CHIP_Init();

  initGPIO();

  // Halt if PB0 held before entering EM3 to allow debug access
  escapeHatch();

  // Power-down MX25 SPI flash
  FlashStatus status;

  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  initACMP();

  // Initialize EM2/3 with default parameters
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_EM23Init(&em23Init);

  // Wait in lowest power state while the comparator runs
  while(1)
  {
    EMU_EnterEM3(false);
  }
}
