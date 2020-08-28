/***************************************************************************//**
 * @file main.c
 * @brief This example demonstrates the TRNG. See readme.txt for details.
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

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_se.h"
#include "retargetserial.h"
#include "mbedtls/entropy_poll.h"
#include <inttypes.h>
#include <stdio.h>

#define mbedtls_printf  printf

#define KEY_SIZE        (32)

// Global variables
static uint8_t randomNum[KEY_SIZE];     // Buffer for random number

/***************************************************************************//**
 * @brief GPIO Interrupt handler for even pins
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_IntClear(1 << BSP_GPIO_PB0_PIN);
}

/***************************************************************************//**
 * @brief Setup GPIO for push button PB0
 ******************************************************************************/
static void setupPushButton0(void)
{
  // Configure PB0 as input and enable interrupt.
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN,
                    false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

/***************************************************************************//**
 * @brief Generate random number through hardware TRNG
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool generateTrngRandom(void)
{
  int ret;                              // Return code
  uint32_t i;
  uint32_t cycles;                      // Cycle counter

  mbedtls_printf("  . Generating random number...");

  // Generate random number with hardware TRNG
  DWT->CYCCNT = 0;
  ret = mbedtls_hardware_poll(NULL, randomNum, KEY_SIZE, (size_t *)&i);
  cycles = DWT->CYCCNT;

  if ((ret != SE_RESPONSE_OK) || (i != KEY_SIZE)) {
    mbedtls_printf(" failed\n  ! mbedtls_hardware_poll returned %d length %d\n",
                   ret, (uint16_t)i);
    return false;
  }

  mbedtls_printf(" ok  (cycles: %" PRIu32 " time: %" PRIu32 " us)",
                 cycles,
                 (cycles * 1000) / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));

  // Print out random number
  for (i = 0; i < KEY_SIZE; i++) {
    if (i % 8 == 0) {
      mbedtls_printf("\n");
    }
    mbedtls_printf("  %d", randomNum[i]);
  }
  mbedtls_printf("\n");
  return true;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  // HFXO kit specific parameters
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Initialize HFXO with kit specific parameters
  CMU_HFXOInit(&hfxoInit);

  // Switch SYSCLK to HFXO
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);

  // Power up trace and debug clocks. Needed for DWT.
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  // Enable DWT cycle counter. Used to measure clock cycles.
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  // Initialize LEUART/USART and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  // Setup PB0
  setupPushButton0();

  // Wait forever in EM1 for PB0 press to generate random number
  while (1) {
    mbedtls_printf("\nCore running at %" PRIu32 " kHz.\n",
                   CMU_ClockFreqGet(cmuClock_HCLK) / 1000);
    mbedtls_printf("Press PB0 to generate %d random numbers (TRNG).\n",
                   KEY_SIZE);

    // Wait key press
    EMU_EnterEM1();

    // Generate random number
    generateTrngRandom();
  }
}
