/***************************************************************************//**
 * @file main.c
 * @brief This example demonstrates the ECDSA. See readme.txt for details.
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

// This file is a modified version of the ecdsa.c example program
// included in mbedtls-2.7.6 (https://tls.mbed.org)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_device.h"
#include "retargetserial.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/entropy.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define mbedtls_printf  printf

// Select specific elliptic curve to use
#define ECPARAMS        MBEDTLS_ECP_DP_SECP256R1

#if !defined(ECPARAMS)
#define ECPARAMS        mbedtls_ecp_curve_list()->grp_id
#endif

// Global variables
static mbedtls_ecdsa_context signCtx;           // ECDSA context for sign
static mbedtls_ecdsa_context verifyCtx;         // ECDSA context for verify
static mbedtls_entropy_context entropy;         // Entropy context
static mbedtls_ctr_drbg_context ctrDrbg;        // CTR_DRBG context
static uint32_t signLen;                        // Length of the signature

// Buffers for hash and signature
static unsigned char messageHash[32];
static unsigned char signature[MBEDTLS_ECDSA_MAX_LEN];
// The message to be signed
static unsigned char msgText[] = "This is the message to be signed.";

/***************************************************************************//**
 * @brief Clear the context structures for ECDSA
 ******************************************************************************/
static void clearEcdsaContext(void)
{
  mbedtls_entropy_free(&entropy);
  mbedtls_ctr_drbg_free(&ctrDrbg);
  mbedtls_ecdsa_free(&signCtx);
  mbedtls_ecdsa_free(&verifyCtx);
}

/***************************************************************************//**
 * @brief Seed random number for ECDSA
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool seedRandomNumber(void)
{
  int ret;                      // Return code
  uint32_t cycles;              // Cycle counter
  const char pers[] = "ecdsa";  // String for CTR_DRBG

  mbedtls_printf("Core running at %" PRIu32 " kHz.\n",
                 CMU_ClockFreqGet(cmuClock_HCLK) / 1000);
  mbedtls_printf("  . Seeding the random number generator...");

  // Initialize entropy and CTR_DRBG contexts
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctrDrbg);

  // Setup the CTR_DRBG entropy source for first seed and future re-seeds
  DWT->CYCCNT = 0;
  if ((ret = mbedtls_ctr_drbg_seed(&ctrDrbg, mbedtls_entropy_func,
                                   &entropy,
                                   (const unsigned char *)pers,
                                   sizeof(pers))) != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    return false;
  }
  cycles = DWT->CYCCNT;

  mbedtls_printf(" ok  (cycles: %" PRIu32 " time: %" PRIu32 " ms)\n",
                 cycles,
                 cycles / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));
  return true;
}

/***************************************************************************//**
 * @brief  SHA-256 Hash
 * @param hashSrc Pointer to message to generate hash
 * @param length The lenght of the message
 * @param hashDst Pointer to hash buffer
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool sha256Hash(unsigned char *hashSrc,
                       size_t length,
                       unsigned char *hashDst)
{
  int ret;                      // Return code
  uint32_t cycles;              // Cycle counter

  DWT->CYCCNT = 0;
  ret = mbedtls_sha256_ret(hashSrc, length, hashDst, 0);
  cycles = DWT->CYCCNT;

  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_sha256_ret returned %d\n", ret);
    return false;
  }
  mbedtls_printf(" ok  (cycles: %" PRIu32 " time: %" PRIu32 " us)\n",
                 cycles,
                 (cycles * 1000) / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));
  return true;
}

/***************************************************************************//**
 * @brief Sign message hash with ECDSA key
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool signMessage(void)
{
  int ret;                      // Return code
  uint32_t cycles;              // Cycle counter

  mbedtls_printf("  . Generating key pair...");

  // Initialize the ECDSA context for signature
  mbedtls_ecdsa_init(&signCtx);

  // Generate an ECDSA key pair for signing
  DWT->CYCCNT = 0;
  if ((ret = mbedtls_ecdsa_genkey(&signCtx, ECPARAMS, mbedtls_ctr_drbg_random,
                                  &ctrDrbg)) != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdsa_genkey returned %d\n", ret);
    return false;
  }
  cycles = DWT->CYCCNT;

  mbedtls_printf(" ok  (key size: %" PRIu16 " bits, cycles: %" PRIu32 " time: %"
                 PRIu32 " ms)\n",
                 (int)signCtx.grp.pbits,
                 cycles,
                 cycles / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));

  // Print out the public key
  if (mbedtls_ecp_point_write_binary(&signCtx.grp, &signCtx.Q,
                                     MBEDTLS_ECP_PF_UNCOMPRESSED,
                                     (size_t *)&cycles, signature,
                                     sizeof(signature)) != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_point_write_binary returned %d\n",
                   ret);
    return false;
  }

  mbedtls_printf("  + Public key (uncompressed point format): ");
  for (ret = 0; ret < cycles; ret++) {
    mbedtls_printf("%c%c", "0123456789ABCDEF"[signature[ret] / 16],
                   "0123456789ABCDEF"[signature[ret] % 16]);
  }

  // Generate message hash (SHA-256)
  mbedtls_printf("\n  . Message: %s\n", msgText);
  mbedtls_printf("  + Computing message (%d bytes) hash...", sizeof(msgText));
  if (!sha256Hash(msgText, sizeof(msgText), messageHash)) {
    return false;
  }

  mbedtls_printf("  . Signing message hash with private key...");
  memset(signature, 0, sizeof(signature));      // Clear signature buffer

  // Compute the ECDSA signature of message
  DWT->CYCCNT = 0;
  if ((ret = mbedtls_ecdsa_write_signature(&signCtx, MBEDTLS_MD_SHA256,
                                           messageHash, sizeof(messageHash),
                                           signature, (size_t *)&signLen,
                                           mbedtls_ctr_drbg_random, &ctrDrbg))
      != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdsa_write_signature returned %d\n",
                   ret);
    return false;
  }
  cycles = DWT->CYCCNT;

  mbedtls_printf(" ok  (signature length: %" PRIu32 ", cycles: %"
                 PRIu32 " time: %" PRIu32 " ms)\n",
                 signLen,
                 cycles,
                 cycles / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));

  // Print out the message and signature
  mbedtls_printf("  + Signature: ");
  for (ret = 0; ret < signLen; ret++) {
    mbedtls_printf("%c%c", "0123456789ABCDEF"[signature[ret] / 16],
                   "0123456789ABCDEF"[signature[ret] % 16]);
  }
  return true;
}

/***************************************************************************//**
 * @brief Verify ECDSA signed message
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool verifySignature(void)
{
  int ret;                      // Return code
  uint32_t cycles;              // Cycle counter

  mbedtls_printf("\n  . Preparing verification context...");

  // Transfer public information to verifying context.
  // We could use the same context for verification and signatures, but we
  // chose to use a new one in order to make it clear that the verifying
  // context only needs the public key (Q), and not the private key (d).

  // Initialize the ECDSA context for verification
  mbedtls_ecdsa_init(&verifyCtx);

  // Copy the contents of group for sign into group for verify
  if ((ret = mbedtls_ecp_group_copy(&verifyCtx.grp, &signCtx.grp)) != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_group_copy returned %d\n", ret);
    return false;
  }

  // Copy the contents of public key (Q) for sign into public key for verify
  if ((ret = mbedtls_ecp_copy(&verifyCtx.Q, &signCtx.Q)) != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_copy returned %d\n", ret);
    return false;
  }

  mbedtls_printf(" ok\n  . Verifying signature with public key...");

  // Read and verify an ECDSA signature
  DWT->CYCCNT = 0;
  if ((ret = mbedtls_ecdsa_read_signature(&verifyCtx, messageHash,
                                          sizeof(messageHash),
                                          signature, signLen)) != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdsa_read_signature returned %d\n",
                   ret);
    return false;
  }
  cycles = DWT->CYCCNT;

  mbedtls_printf(" ok  (cycles: %" PRIu32 " time: %" PRIu32 " ms)\n",
                 cycles,
                 cycles / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));
  return true;
}

/***************************************************************************//**
 * @brief Main function
 ******************************************************************************/
int main(void)
{
  // HFXO kit specific parameters
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Initialize DCDC regulator if available
  #if defined(_EMU_DCDCCTRL_MASK) || defined(_DCDC_CTRL_MASK)
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  #endif

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

  // Start ECDSA process
  // Seed entropy source
  if (!seedRandomNumber()) {
    goto cleanup;
  }

  // Sign message
  if (!signMessage()) {
    goto cleanup;
  }

  // Verify signed message
  verifySignature();

  // Clean up before exit
  cleanup:
  // Clear contexts to exit
  clearEcdsaContext();
  while (1)
    ;
}
