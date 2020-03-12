/***************************************************************************//**
 * @file config-sl-crypto-test-acceleration.h
 * @brief Configuration for enabling CRYPTO hardware acceleration in mbed TLS.
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef CONFIG_SL_CRYPTO_TEST_ACCELERATION_H
#define CONFIG_SL_CRYPTO_TEST_ACCELERATION_H

/**
 * @addtogroup sl_crypto
 * @{
 * @addtogroup sl_crypto_config Cryptography Hardware Acceleration Configuration
 *
 * @brief
 *  Configuration macros for Silicon Labs CRYPTO hardware acceleration mbed TLS plugins.
 *
 * @details
 *  The config-sl-crypto-test-acceleration.h file lists configuration macros for setup of the crypto hardware accelerator plugins for mbed TLS from Silicon Labs. You can use macros in config-sl-crypto-test-acceleration.h and mbedtls/include/mbedtls/config.h in order to configure your mbed TLS application running on Silicon Labs devices.
 *
 * @warning
 *  This configuration file should be used as a starting point only for hardware acceleration evaluation on Silicon Labs devices.
 * @{
 */

#include "em_device.h"

#if !defined(NO_CRYPTO_ACCELERATION)

/**
 * \def MBEDTLS_AES_ALT
 *
 * Enable hardware acceleration for the AES block cipher
 *
 * Module:  sl_crypto/src/crypto_aes.c for devices with CRYPTO
 *          sl_crypto/src/aes_aes.c for devices with AES
 *
 * See MBEDTLS_AES_C for more information.
 */
#define MBEDTLS_AES_ALT

/**
 * \def MBEDTLS_ECP_INTERNAL_ALT
 * \def ECP_SHORTWEIERSTRASS
 * \def MBEDTLS_ECP_ADD_MIXED_ALT
 * \def MBEDTLS_ECP_DOUBLE_JAC_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_ALT
 *
 * Enable hardware acceleration for the elliptic curve over GF(p) library.
 *
 * Module:  sl_crypto/src/crypto_ecp.c
 * Caller:  library/ecp.c
 *
 * Requires: MBEDTLS_BIGNUM_C, MBEDTLS_ECP_C and at least one
 * MBEDTLS_ECP_DP_XXX_ENABLED and (CRYPTO_COUNT > 0)
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_ECP_INTERNAL_ALT
#define ECP_SHORTWEIERSTRASS
#define MBEDTLS_ECP_ADD_MIXED_ALT
#define MBEDTLS_ECP_DOUBLE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT
#define MBEDTLS_ECP_RANDOMIZE_JAC_ALT
#endif

/**
 * \def MBEDTLS_SHA1_ALT
 *
 * Enable hardware acceleration for the SHA1 cryptographic hash algorithm.
 *
 * Module:  sl_crypto/src/crypto_sha.c
 * Caller:  library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *          library/x509write_crt.c
 *
 * Requires: MBEDTLS_SHA1_C and (CRYPTO_COUNT > 0)
 * See MBEDTLS_SHA1_C for more information.
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_SHA1_ALT
#endif

/**
 * \def MBEDTLS_SHA256_ALT
 *
 * Enable hardware acceleration for the SHA-224 and SHA-256 cryptographic
 * hash algorithms.
 *
 * Module:  sl_crypto/src/crypto_sha.c
 * Caller:  library/entropy.c
 *          library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *
 * Requires: MBEDTLS_SHA256_C and (CRYPTO_COUNT > 0)
 * See MBEDTLS_SHA256_C for more information.
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_SHA256_ALT
#endif

#if defined(CRYPTOACC_PRESENT)
#define MBEDTLS_AES_ALT
#define AES_192_SUPPORTED
#define MBEDTLS_CCM_ALT
#define MBEDTLS_CMAC_ALT
#define MBEDTLS_ECDH_COMPUTE_SHARED_ALT
#define MBEDTLS_GCM_ALT
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA256_ALT

#define MBEDTLS_ECDH_GEN_PUBLIC_ALT
#define MBEDTLS_ECDSA_GENKEY_ALT
#define MBEDTLS_ECDSA_SIGN_ALT
#define MBEDTLS_ECDSA_VERIFY_ALT
#endif /* CRYPTOACC */

#if defined(SEMAILBOX_PRESENT)
#include "em_se.h"

#if defined(SE_COMMAND_OPTION_HASH_SHA1)
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA1_PROCESS_ALT
#endif
#if defined(SE_COMMAND_OPTION_HASH_SHA256) || defined(SE_COMMAND_OPTION_HASH_SHA224)
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_SHA256_PROCESS_ALT
#endif
#if defined(SE_COMMAND_OPTION_HASH_SHA512) || defined(SE_COMMAND_OPTION_HASH_SHA384)
#define MBEDTLS_SHA512_ALT
#define MBEDTLS_SHA512_PROCESS_ALT
#endif
#if defined(SE_COMMAND_CREATE_KEY)
#define MBEDTLS_ECDH_GEN_PUBLIC_ALT
#define MBEDTLS_ECDSA_GENKEY_ALT
#endif
#if defined(SE_COMMAND_DH)
#define MBEDTLS_ECDH_COMPUTE_SHARED_ALT
#endif
#if defined(SE_COMMAND_SIGNATURE_SIGN)
#define MBEDTLS_ECDSA_SIGN_ALT
#endif
#if defined(SE_COMMAND_SIGNATURE_VERIFY)
#define MBEDTLS_ECDSA_VERIFY_ALT
#endif
#if defined(SE_COMMAND_JPAKE_GEN_SESSIONKEY)
#define MBEDTLS_ECJPAKE_ALT
#endif

#if defined(SE_COMMAND_AES_CCM_ENCRYPT) && defined(SE_COMMAND_AES_CCM_DECRYPT)
#define MBEDTLS_CCM_ALT
#endif
#if defined(SE_COMMAND_AES_GCM_ENCRYPT) && defined(SE_COMMAND_AES_GCM_DECRYPT)
#define MBEDTLS_GCM_ALT
#endif
#if defined(SE_COMMAND_AES_CMAC)
#define MBEDTLS_CMAC_ALT
#endif
#endif /* SEMAILBOX_PRESENT */

#endif /* !NO_CRYPTO_ACCELERATION */

#define MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_ENTROPY_FORCE_SHA256

/**
 * \def MBEDTLS_TRNG_C
 *
 * Enable software support for the True Random Number Generator (TRNG)
 * incorporated from Series 1 Configuration 2 devices (EFR32MG12, etc.)
 * from Silicon Labs.
 *
 * TRNG is not supported by software for EFR32XG13 (SDID_89) and
 * EFR32XG14 (SDID_95).
 *
 * Requires TRNG_PRESENT &&
 *  !(_SILICON_LABS_GECKO_INTERNAL_SDID_95)
 */
#if defined(TRNG_PRESENT)                           \
  && !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_95) \
  && !defined(_SILICON_LABS_GECKO_INTERNAL_SDID_89)
#define MBEDTLS_TRNG_C
#endif

/**
 * \def MBEDTLS_ENTROPY_ADC_C
 *
 * Enable software support for the retrieving entropy data from the ADC
 * incorporated on devices from Silicon Labs.
 *
 * Requires ADC_PRESENT && _ADC_SINGLECTRLX_VREFSEL_VENTROPY &&
 *          _SILICON_LABS_32B_SERIES_1
 */
#if defined(ADC_PRESENT)                        \
  && defined(_ADC_SINGLECTRLX_VREFSEL_VENTROPY) \
  && defined(_SILICON_LABS_32B_SERIES_1)
#define MBEDTLS_ENTROPY_ADC_C
#endif

/**
 * \def MBEDTLS_ENTROPY_ADC_INSTANCE
 *
 * Specify which ADC instance shall be used as entropy source.
 *
 * Requires MBEDTLS_ENTROPY_ADC_C
 */
#if defined(MBEDTLS_ENTROPY_ADC_C)
#define MBEDTLS_ENTROPY_ADC_INSTANCE  (0)
#endif

/**
 * \def MBEDTLS_ENTROPY_RAIL_C
 *
 * Enable software support for the retrieving entropy data from the RAIL
 * incorporated on devices from Silicon Labs.
 *
 * Requires _EFR_DEVICE
 */
#if defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1)
#define MBEDTLS_ENTROPY_RAIL_C
#endif

/**
 * \def MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL
 *
 * Use the radio (RAIL) as default hardware entropy source.
 *
 * Requires MBEDTLS_ENTROPY_RAIL_C && _SILICON_LABS_32B_SERIES_1 &&
 *         !MBEDTLS_TRNG_C
 */
#if defined(MBEDTLS_ENTROPY_RAIL_C)      \
  && defined(_SILICON_LABS_32B_SERIES_1) \
  && !defined(MBEDTLS_TRNG_C)
#define MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL
#endif

/**
 * \def MBEDTLS_ENTROPY_HARDWARE_ALT
 *
 * Integrate the provided default entropy source into the mbed
 * TLS entropy infrastructure.
 *
 * Requires MBEDTLS_TRNG_C || MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL || SEMAILBOX
 */
#if defined(MBEDTLS_TRNG_C) || defined(MBEDTLS_ENTROPY_HARDWARE_ALT_RAIL) || defined(SEMAILBOX_PRESENT)
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif

/* Default ECC configuration for Silicon Labs devices: */

/* ECC curves supported by CRYPTO hardware module: */
//#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
//#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS   256
#ifndef MBEDTLS_MPI_MAX_SIZE
#define MBEDTLS_MPI_MAX_SIZE    32 // 384 bits is 48 bytes
#endif

/*
   Set MBEDTLS_ECP_WINDOW_SIZE to configure
   ECC point multiplication window size, see ecp.h:
   2 = Save RAM at the expense of speed
   3 = Improve speed at the expense of RAM
   4 = Optimize speed at the expense of RAM
 */
#define MBEDTLS_ECP_WINDOW_SIZE        3
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

/* Significant speed benefit at the expense of some ROM */
#define MBEDTLS_ECP_NIST_OPTIM

/* Include the default mbed TLS config file */
#include "mbedtls/config.h"

#undef MBEDTLS_ERROR_C    
#undef MBEDTLS_TIMING_C
#undef MBEDTLS_FS_IO
#define MBEDTLS_NO_PLATFORM_ENTROPY

/* Exclude and/or change default config here. E.g.: */
#undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
#undef MBEDTLS_ECP_DP_BP384R1_ENABLED
#undef MBEDTLS_ECP_DP_BP512R1_ENABLED
#undef MBEDTLS_SHA512_C

#include "mbedtls/check_config.h"

/** @} (end sl_crypto_config) */
/** @} (end sl_crypto) */

#endif /* CONFIG_SL_CRYPTO_TEST_ACCELERATION_H */
