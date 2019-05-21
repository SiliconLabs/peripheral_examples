/***************************************************************************//**
 * @file
 * @brief This example demonstrates the ECDH.
 *        See readme.txt for details.
 * @version 0.0.1
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-ms/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// This file is a modified version of the ecdh_curve25519.c example program
// included in mbedtls-2.7.6 (https://tls.mbed.org)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "retargetserial.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/entropy.h"
#include "mbedtls/timing.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define mbedtls_printf  printf

#define CLIENT          true
#define SERVER          false
#define KEY_SIZE        32

// Select specific elliptic curve to use
#define MBEDTLS_ECC_ID  (MBEDTLS_ECP_DP_SECP256R1)

// Global variables
static mbedtls_ecdh_context clientCtx;          // ECDH client context
static mbedtls_ecdh_context serverCtx;          // ECDH server context
static mbedtls_entropy_context entropy;         // Entropy context
static mbedtls_ctr_drbg_context ctrDrbg;        // CTR_DRBG context
static unsigned char clientXtoServer[KEY_SIZE]; // Buffers for keys
static unsigned char clientYtoServer[KEY_SIZE];
static unsigned char serverXtoClient[KEY_SIZE];
static unsigned char serverYtoClient[KEY_SIZE];

/***************************************************************************//**
 * @brief Clear the context structures for ECDH
 ******************************************************************************/
static void clearEcdhContext(void)
{
  mbedtls_entropy_free(&entropy);
  mbedtls_ctr_drbg_free(&ctrDrbg);
  mbedtls_ecdh_free(&clientCtx);
  mbedtls_ecdh_free(&serverCtx);
}

/***************************************************************************//**
 * @brief Seed random number for ECDH
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool seedRandomNumber(void)
{
  int ret;                      // Return code
  uint32_t cycles;              // Cycle counter
  const char pers[] = "ecdh";   // String for CTR_DRBG

  mbedtls_printf("Core running at %" PRIu32 " kHz.\n",
                 CMU_ClockFreqGet(cmuClock_HCLK) / 1000);
  mbedtls_printf("  . Seeding the random number generator...");

  // Initialize entropy and CTR_DRBG contexts
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctrDrbg);

  // Setup the CTR_DRBG entropy source for first seed and future reseeds
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
 * @brief Setup ECDH for client and server
 * @param mode True for client, false for server
 * @param peerCtr Pointer to ECDH context
 * @param ecpPointX Pointer to ECP point X buffer
 * @param ecpPointY Pointer to ECP point Y buffer
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool setupEcdhPeer(bool mode,
                          mbedtls_ecdh_context *peerCtx,
                          unsigned char *ecpPointX,
                          unsigned char *ecpPointY)
{
  int ret;                      // Return code
  uint32_t cycles;              // Cycle counter

  if (mode) {
    mbedtls_printf("  . Setting up client context...");
  } else {
    mbedtls_printf("  . Setting up server context...");
  }

  // Initialize the ECDH context
  mbedtls_ecdh_init(peerCtx);

  // Setup an ECP (Elliptic Curves over Prime) group context
  // from a specific elliptic curve (MBEDTLS_ECC_ID)
  ret = mbedtls_ecp_group_load(&peerCtx->grp, MBEDTLS_ECC_ID);
  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecp_group_load returned %d\n", ret);
    return false;
  }

  // Generate an ECDH keypair on an elliptic curve
  // Private key in peerCtx.d and public key in peerCtx.Q
  DWT->CYCCNT = 0;
  ret = mbedtls_ecdh_gen_public(&peerCtx->grp, &peerCtx->d, &peerCtx->Q,
                                mbedtls_ctr_drbg_random, &ctrDrbg);
  cycles = DWT->CYCCNT;

  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_gen_public returned %d\n", ret);
    return false;
  }

  // Convert X coordinate of the ECP point into unsigned binary data
  ret = mbedtls_mpi_write_binary(&peerCtx->Q.X, ecpPointX, KEY_SIZE);
  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    return false;
  }

  // Convert Y coordinate of the ECP point into unsigned binary data
  ret = mbedtls_mpi_write_binary(&peerCtx->Q.Y, ecpPointY, KEY_SIZE);
  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    return false;
  }

  mbedtls_printf(" ok  (key size: %" PRIu16 " bits, cycles: %" PRIu32 " time: %"
                 PRIu32 " ms)\n",
                 (int)peerCtx->grp.pbits,
                 cycles,
                 cycles / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));
  return true;
}

/***************************************************************************//**
 * @brief Use peer's key to generate shared secret
 * @param mode True to use client key, false to use server key
 * @param peerCtr Pointer to ECDH context
 * @param ecpPointX Pointer to ECP point X buffer
 * @param ecpPointY Pointer to ECP point Y buffer
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool generatePeerKeySecret(bool mode,
                                  mbedtls_ecdh_context *peerCtx,
                                  unsigned char *ecpPointX,
                                  unsigned char *ecpPointY)
{
  int ret;                      // Return code
  uint32_t cycles;              // Cycle counter

  if (mode) {
    mbedtls_printf("  . Server reading client key and computing secret...");
  } else {
    mbedtls_printf("  . Client reading server key and computing secret...");
  }

  // Read peer's key and generate shared secret
  ret = mbedtls_mpi_lset(&peerCtx->Qp.Z, 1);
  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_lset returned %d\n", ret);
    return false;
  }

  // Read peer's X coordinate of the ECP point
  ret = mbedtls_mpi_read_binary(&peerCtx->Qp.X, ecpPointX, KEY_SIZE);
  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    return false;
  }

  // Read peer's Y coordinate of the ECP point
  ret = mbedtls_mpi_read_binary(&peerCtx->Qp.Y, ecpPointY, KEY_SIZE);
  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_read_binary returned %d\n", ret);
    return false;
  }

  // Compute the shared secret to peerCtx.z
  DWT->CYCCNT = 0;
  ret = mbedtls_ecdh_compute_shared(&peerCtx->grp, &peerCtx->z,
                                    &peerCtx->Qp, &peerCtx->d,
                                    mbedtls_ctr_drbg_random, &ctrDrbg);
  cycles = DWT->CYCCNT;

  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_ecdh_compute_shared returned %d\n",
                   ret);
    return false;
  }

  mbedtls_printf(" ok  (cycles: %" PRIu32 " time: %" PRIu32 " ms)\n",
                 cycles,
                 cycles / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));
  return true;
}

/***************************************************************************//**
 * @brief Verify shared secret between client and server
 * @return true if successful and false otherwise.
 ******************************************************************************/
static bool verifyPeerSecret(void)
{
  int ret;                      // Return code
  unsigned char buf[KEY_SIZE];  // Buffer for shared secret binary

  mbedtls_printf("  . Checking if both computed secrets are equal...");

  // Verification: are the computed secret equal?
  ret = mbedtls_mpi_cmp_mpi(&clientCtx.z, &serverCtx.z);
  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_cmp_mpi returned %d\n",
                   ret);
    return false;
  }
  mbedtls_printf(" ok\n");

  // Convert the shared secret into binary
  ret = mbedtls_mpi_write_binary(&clientCtx.z, buf, sizeof(buf));
  if (ret != 0) {
    mbedtls_printf(" failed\n  ! mbedtls_mpi_write_binary returned %d\n", ret);
    return false;
  }

  // Print out the shared secret
  mbedtls_printf("  + Shared Secret: ");
  for (ret = 0; ret < sizeof(buf); ret++) {
    mbedtls_printf("%c%c", "0123456789ABCDEF"[buf[ret] / 16],
                   "0123456789ABCDEF"[buf[ret] % 16]);
  }
  mbedtls_printf("\n");
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

  // Start ECDH process
  // Seed entropy source
  if (!seedRandomNumber()) {
    goto cleanup;
  }

  // Setup ECDH client
  if (!setupEcdhPeer(CLIENT, &clientCtx, clientXtoServer, clientYtoServer)) {
    goto cleanup;
  }

  // Setup ECDH server
  if (!setupEcdhPeer(SERVER, &serverCtx, serverXtoClient, serverYtoClient)) {
    goto cleanup;
  }

  // Use client key to generate shared secret
  if (!generatePeerKeySecret(CLIENT, &serverCtx, clientXtoServer,
                             clientYtoServer)) {
    goto cleanup;
  }

  // Use server key to generate shared secret
  if (!generatePeerKeySecret(SERVER, &clientCtx, serverXtoClient,
                             serverYtoClient)) {
    goto cleanup;
  }

  // Verify shared secret
  verifyPeerSecret();

  // Clean up before exit
  cleanup:
  // Clear contexts to exit
  clearEcdhContext();
  while (1)
    ;
}
