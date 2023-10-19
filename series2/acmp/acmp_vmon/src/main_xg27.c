/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the usage of the ACMP module in combination
 * with the VSENSE inputs to monitor AVDD.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_emu.h"
#include "em_cmu.h"
#include "em_acmp.h"
#include "em_gpio.h"
#include "bsp.h"

#define LED0_PORT           BSP_GPIO_LED0_PORT
#define LED0_PIN            BSP_GPIO_LED0_PIN

#define LED1_PORT           BSP_GPIO_LED1_PORT
#define LED1_PIN            BSP_GPIO_LED1_PIN

#define ACMP_OUT_PORT       gpioPortC
#define ACMP_OUT_PIN        0

uint32_t AComp0 = 0;

// Flag to keep the state of the power rails 
volatile bool AVDD_good = true;

/**************************************************************************//**
 * @brief AMP0 Interrupt handler
 *****************************************************************************/
void ACMP0_IRQHandler(void)
{
  // Clear interrupt flag 
  ACMP0->IF_CLR = ACMP_IF_ACMPRDY;
  ACMP0->IF_CLR = ACMP_IF_FALL;
  ACMP0->IF_CLR = ACMP_IF_RISE;

  // Get output 
  AComp0 = ((ACMP0->STATUS & ACMP_STATUS_ACMPOUT) >> _ACMP_STATUS_ACMPOUT_SHIFT);
  
  if (AComp0 == 0) {
    AVDD_good = false; // VSENSE is below threshold
    GPIO_PinOutClear(LED0_PORT, LED0_PIN);  // For BRD4194A, LED = 0 means LED is turned on
    GPIO_PinOutSet(LED1_PORT, LED1_PIN);
  } else {
    AVDD_good = true; // VSENSE is above threshold
    GPIO_PinOutClear(LED1_PORT, LED1_PIN);  // For BRD4194A, LED = 0 means LED is turned on
    GPIO_PinOutSet(LED0_PORT, LED0_PIN);
  }
}

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
 * @brief  Main function
 *
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
  // Chip errata 
  CHIP_Init();

  // Recommended recovery procedure for code in development
  escapeHatch();

  // Initialize ACMP settings 
  ACMP_Init_TypeDef acmp0Init =
  {
    0x2,                      // Using biasProg value of 0x2. 
    acmpInputRangeReduced,    // Reduced 0 to Vdd-0.7. 
    acmpAccuracyHigh,         // High accuracy, less current usage. 
    acmpHysteresisDisabled,   // Disable hysteresis. 
    false,                    // Output 0 when ACMP is inactive. 
    0x10,                     // Set VREFDIV. 
    true                      // Enable after init. 
  };

  CMU_ClockEnable(cmuClock_ACMP0, true);  // Enable ACMP0 clock
  CMU_ClockEnable(cmuClock_GPIO, true);  // Enable GPIO clock

  ACMP_Init(ACMP0, &acmp0Init);

  // Set VREF 2.5V to NEGSEL input and VSENSE0 to POSSEL input 
  ACMP_ChannelSet(ACMP0, acmpInputVREFDIV2V5, acmpInputVSENSE01DIV4);

  // Enable rising/falling/ready interrupts 
  ACMP_IntEnable(ACMP0, ACMP_IEN_RISE);
  ACMP_IntEnable(ACMP0, ACMP_IEN_FALL);
  ACMP_IntEnable(ACMP0, ACMP_IEN_ACMPRDY);

  // Wait for warm up 
  while (!(ACMP0->STATUS & ACMP_STATUS_ACMPRDY));

  // Configure LEDs 
  GPIO_PinModeSet(LED0_PORT, LED0_PIN , gpioModePushPull, 0);
  GPIO_PinModeSet(LED1_PORT, LED1_PIN, gpioModePushPull, 0);

  /* To be able to probe the output we can send the ACMP output to a pin.
   * The second argument to this function is the pin location which is
   * device dependent. */
  GPIO_PinModeSet(ACMP_OUT_PORT, ACMP_OUT_PIN, gpioModePushPull, 0);
  ACMP_GPIOSetup(ACMP0, ACMP_OUT_PORT, ACMP_OUT_PIN, true, false);

  // Enable interrupts 
  NVIC_ClearPendingIRQ(ACMP0_IRQn);
  NVIC_EnableIRQ(ACMP0_IRQn);

  while (1) {
    // Enter EM3 until the next interrupt occurs 
    EMU_EnterEM3(true);
  }
}
