/***************************************************************************//**
 * @file
 * @brief This example demonstrates the TRNG with mbed TLS entropy functions.
 *        See readme.txt for details.
 * @version 0.0.1
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The license of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "retargetserial.h"
#include "mbedtls/entropy.h"
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
 * @brief Generate random number through TRNG with mbed TLS entropy functions
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool generateEntropyRandom(void)
{
  int ret;                              // Return code
  uint32_t i;
  uint32_t cycles;                      // Cycle counter
  mbedtls_entropy_context entropy;      // Entropy context

  mbedtls_printf("  . Generating random number...");

  // Initialize and gather entropy sources
  mbedtls_entropy_init(&entropy);
  mbedtls_entropy_gather(&entropy);

  // Generate random number with mbedtls_entropy_func()
  DWT->CYCCNT = 0;
  for (i = 0; i < KEY_SIZE / MBEDTLS_ENTROPY_BLOCK_SIZE; i++) {
    if ((ret = mbedtls_entropy_func(&entropy,
                                    &randomNum[i * MBEDTLS_ENTROPY_BLOCK_SIZE],
                                    MBEDTLS_ENTROPY_BLOCK_SIZE)) != 0) {
      mbedtls_printf(" failed\n  ! mbedtls_entropy_func returned %d\n", ret);
      return false;
    }
  }
  cycles = DWT->CYCCNT;

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

  // Clear entropy context to exit
  mbedtls_entropy_free(&entropy);
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
    mbedtls_printf("Press PB0 to generate %d random numbers "
                   "(TRNG + mbed TLS).\n", KEY_SIZE);

    // Wait key press
    EMU_EnterEM1();

    // Generate random number
    generateEntropyRandom();
  }
}
