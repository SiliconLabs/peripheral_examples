/***************************************************************************//**
 * @file
 * @brief This example demonstrates the AES-256 encryption and decryption.
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

// This example is a modified version of the aescrypt2.c example program
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
#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define mbedtls_printf                  printf

#define MODE_ENCRYPT                    (1)
#define MODE_DECRYPT                    (2)
#define CARRIAGE_RETURN                 (13)

#define IV_SIZE                         (16)
#define TAG_SIZE                        (32)
#define KEY_SIZE                        (32)
#define AES_BLOCK_SIZE                  (16)
#define MAX_MESSAGE_SIZE_ENCRYPTION     (1024)
#define MAX_MESSAGE_SIZE_DECRYPTION \
  (2 * MAX_MESSAGE_SIZE_ENCRYPTION + 2 * IV_SIZE + 2 * TAG_SIZE + 1)

// Global variables
static char initialVector[IV_SIZE];
static uint32_t messageSize, maxMessageSize;
static char message[MAX_MESSAGE_SIZE_DECRYPTION];

// Hard-coded 256-bit key for AES encryption and decryption.
static const char *aesKey256 = "603DEB10 15CA71BE 2B73AEF0 857D7781"
                               "1F352C07 3B6108D7 2D9810A3 0914DFF4";

/***************************************************************************//**
 * @brief Convert ASCII hexadecimal text into binary
 * @param binBuf Pointer to converted binary data
 * @param binBufLen Size of the binary data in bytes
 * @param *hexStr Pointer to hexadecimal text for conversion
 * @return Number of bytes to be converted
 ******************************************************************************/
static int hexToBin(uint8_t *binBuf, unsigned int binBufLen, const char *hexStr)
{
  int i;
  uint8_t tmp;
  uint8_t val;
  uint32_t ret = 0;

  while (ret < binBufLen) {
    val = 0;
    for (i = 1; i >= 0; i--) {
      tmp = *hexStr;
      hexStr++;
      // Skip spaces
      while (tmp == ' ') {
        tmp = *hexStr;
        hexStr++;
      }

      // Reached end of string?
      if (!tmp) {
        return ret;
      }

      if (tmp > '9') {
        // Ensure upper case hex
        tmp &= ~0x20;
        val |= ((tmp - 'A') + 10) << (4 * i);
      } else {
        val |= (tmp - '0') << (4 * i);
      }
    }
    *binBuf = val;
    binBuf++;
    ret++;
  }
  return ret;
}

/***************************************************************************//**
 * @brief Convert binary data to ASCII hexadecimal text string
 * @param *hexStr Pointer to converted hexadecimal text
 * @param *binBuf Pointer to binary data for conversion
 * @param binBufLen Size of the binary data in bytes
 ******************************************************************************/
static void binToHex(char *hexStr, uint8_t *binBuf, unsigned int binBufLen)
{
  uint32_t i;
  uint8_t nibble;

  for (i = 0; i < binBufLen; i++) {
    // Convert high nibble
    nibble = (binBuf[i] >> 4) & 0x0F;
    if (nibble > 9) {
      *hexStr = nibble - 10 + 'A';
    } else {
      *hexStr = nibble + '0';
    }
    hexStr++;

    // Convert low nibble
    nibble = binBuf[i] & 0x0F;
    if (nibble > 9) {
      *hexStr = nibble - 10 + 'A';
    } else {
      *hexStr = nibble + '0';
    }
    hexStr++;
  }
  // Null terminate at end of string.
  *hexStr = 0;
}

/***************************************************************************//**
 * @brief Select AES encryption or decryption
 * @return AES mode, MODE_ENCRYPT or MODE_DECRYPT
 ******************************************************************************/
static int selectAesMode(void)
{
  int mode = 0;         // AES mode
  int keyInput;         // Key input buffer
  char *bufPtr;         // Pointer to buffer

  // Fill buffers with null character
  memset(initialVector, 0, IV_SIZE);
  memset(message, 0, MAX_MESSAGE_SIZE_DECRYPTION);

  // Wait key press to select mode
  while (mode == 0) {
    keyInput = getchar();

    // Key 'e' press?
    if (keyInput == 'e') {
      putchar(keyInput);        // Local echo
      mbedtls_printf("\n");

      mode = MODE_ENCRYPT;
      maxMessageSize = MAX_MESSAGE_SIZE_ENCRYPTION;
      mbedtls_printf("Thanks. Please type a short phrase to be used as input "
                     "to generate the initial vector of the encryption, "
                     "terminated by entering maximum 16 characters or hitting "
                     "<Enter>: ");

      // Get data to generate an initial vector (IV) for encryption
      messageSize = 0;
      bufPtr = initialVector;

      while (messageSize < IV_SIZE) {
        keyInput = getchar();

        // Newline marks end of message
        if (keyInput == CARRIAGE_RETURN) {
          break;
        }
        if (keyInput > 0) {
          *bufPtr = keyInput;
          bufPtr++;
          messageSize++;
          putchar(keyInput);    // Local echo
        }
      }

      mbedtls_printf("\nThanks. Please send a message to be encrypted, "
                     "terminated by entering maximum %u characters or hitting "
                     "<Enter>. The ciphertext will be printed in the following "
                     "format:\n",
                     MAX_MESSAGE_SIZE_ENCRYPTION);
      keyInput = 'e';           // Make sure input is not 'd'
    }

    // Key 'd' press?
    if (keyInput == 'd') {
      putchar(keyInput);        // Local echo
      mbedtls_printf("\n");

      mode = MODE_DECRYPT;
      maxMessageSize = MAX_MESSAGE_SIZE_DECRYPTION;
      mbedtls_printf("Thanks. Please send the message (ciphertext) to be "
                     "decrypted. The format must be:\n");
    }
  }

  mbedtls_printf("Initial Vector (16 bytes)|"
                 "Ciphertext (maximum %u bytes)|"
                 "Message Digest Tag (32 bytes)\n",
                 MAX_MESSAGE_SIZE_ENCRYPTION);

  // Get data for encryption or decryption
  messageSize = 0;
  bufPtr = message;

  while (messageSize < maxMessageSize) {
    keyInput = getchar();

    // Newline marks end of message
    if (keyInput == CARRIAGE_RETURN) {
      break;
    }
    if (keyInput > 0) {
      *bufPtr = keyInput;
      bufPtr++;
      messageSize++;
      putchar(keyInput);        // Local echo
    }
  }

  // Newline after plain text message input, or ciphertext.
  mbedtls_printf("\n");
  return mode;
}

/***************************************************************************//**
 * @brief AES encryption
 ******************************************************************************/
static void aesEncrypt(void)
{
  int i;
  char *bufPtr;                         // Pointer to buffer
  uint32_t offset;                      // AES block offset
  uint32_t cycles;                      // Cycle counter
  char hexBuf[2 * TAG_SIZE + 1];        // Hex buffer, +1 for null character
  unsigned char digest[TAG_SIZE];       // Buffer for hash value
  unsigned char buffer[TAG_SIZE];       // Generic storage

  mbedtls_aes_context aesCtx;           // AES context
  mbedtls_md_context_t shaCtx;          // SHA context

  // Initialize AES and SHA contexts
  mbedtls_aes_init(&aesCtx);
  mbedtls_md_init(&shaCtx);

  // Use SHA256 and HMAC for message digest
  i = mbedtls_md_setup(&shaCtx,
                       mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                       1);
  if (i != 0) {
    // Clear AES and SHA contexts to exit
    mbedtls_aes_free(&aesCtx);
    mbedtls_md_free(&shaCtx);
    mbedtls_printf("  ! mbedtls_md_setup() returned -0x%04x\n", -i);
    return;
  }

  // Generate the initialization vector (IV) as:
  // IV[0..15] = SHA-256(messageSize[0..7] || initialVector[0..strlen])
  for (i = 0; i < 8; i++) {
    buffer[i] = (unsigned char)(messageSize >> (i << 3));
  }

  // Start and feed data into the message-digest computation
  mbedtls_md_starts(&shaCtx);
  mbedtls_md_update(&shaCtx, buffer, 8);
  mbedtls_md_update(&shaCtx,
                    (unsigned char*)initialVector,
                    strlen(initialVector));

  // Finish the digest operation
  mbedtls_md_finish(&shaCtx, digest);

  // The last nibble in the IV are actually used
  // to store the file size modulo the AES block size.
  digest[IV_SIZE - 1] = (digest[IV_SIZE - 1] & 0xF0) | (messageSize & 0x0F);

  // Copy the result to IV[0..15]
  memcpy(initialVector, digest, IV_SIZE);

  // Append the IV at the beginning of the output.
  binToHex(hexBuf, (uint8_t *)initialVector, IV_SIZE);
  mbedtls_printf(hexBuf);

  // The digest[0..15] = IV[0..15] and digest[16..31] = 0
  memset((digest + IV_SIZE), 0, IV_SIZE);

  // Convert ASCII hard-coded key into binary
  hexToBin((uint8_t *)buffer, KEY_SIZE, aesKey256);

  DWT->CYCCNT = 0;
  // Hash the IV and the secret key together 8192 times
  for (i = 0; i < 8192; i++) {
    mbedtls_md_starts(&shaCtx);
    mbedtls_md_update(&shaCtx, digest, TAG_SIZE);
    mbedtls_md_update(&shaCtx, buffer, KEY_SIZE);
    mbedtls_md_finish(&shaCtx, digest);
  }

  // Using the result in digest as key to setup the AES and HMAC.
  mbedtls_aes_setkey_enc(&aesCtx, digest, KEY_SIZE * 8);
  mbedtls_md_hmac_starts(&shaCtx, digest, TAG_SIZE);

  // Encrypt plain text and write the ciphertext.
  for (bufPtr = message, offset = 0;
       offset < messageSize;
       offset += AES_BLOCK_SIZE, bufPtr += AES_BLOCK_SIZE) {
    // Copy the plain text to buffer
    if ((messageSize - offset) > AES_BLOCK_SIZE) {
      memcpy(buffer, bufPtr, AES_BLOCK_SIZE);
    } else {
      // Fill null character (0) on last block if not equal to AES_BLOCK_SIZE
      memset(buffer, 0, AES_BLOCK_SIZE);
      memcpy(buffer, bufPtr, (messageSize - offset));
    }

    // Plain text XOR IV
    for (i = 0; i < AES_BLOCK_SIZE; i++) {
      buffer[i] = (unsigned char)(buffer[i] ^ initialVector[i]);
    }

    // AES encrypt then HMAC on encrypted text
    mbedtls_aes_crypt_ecb(&aesCtx, MBEDTLS_AES_ENCRYPT, buffer, buffer);
    mbedtls_md_hmac_update(&shaCtx, buffer, AES_BLOCK_SIZE);

    // Use the encrypted text to XOR the following plain text
    memcpy(initialVector, buffer, AES_BLOCK_SIZE);

    // Print out ciphertext
    binToHex(hexBuf, buffer, AES_BLOCK_SIZE);
    mbedtls_printf(hexBuf);
  }

  // Finally write the HMAC.
  mbedtls_md_hmac_finish(&shaCtx, digest);
  cycles = DWT->CYCCNT;

  // Print out digest, cycles and time
  binToHex(hexBuf, digest, TAG_SIZE);
  mbedtls_printf("%s\n", hexBuf);
  mbedtls_printf("Encryption, cycles: %" PRIu32 " time: %" PRIu32 " ms\n",
                 cycles,
                 cycles / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));

  // Clear AES and SHA contexts to exit
  mbedtls_aes_free(&aesCtx);
  mbedtls_md_free(&shaCtx);
}

/***************************************************************************//**
 * @brief AES decryption
 ******************************************************************************/
static void aesDecrypt(void)
{
  int i;
  char *bufPtr;                         // Pointer to buffer
  uint32_t offset;                      // AES block offset
  uint32_t cycles;                      // Cycle counter
  unsigned char digest[TAG_SIZE];       // Buffer for hash value
  unsigned char buffer[TAG_SIZE];       // Generic storage

  mbedtls_aes_context aesCtx;           // AES context
  mbedtls_md_context_t shaCtx;          // SHA context

  // Initialize AES and SHA contexts
  mbedtls_aes_init(&aesCtx);
  mbedtls_md_init(&shaCtx);

  // Use SHA256 and HMAC for message digest
  i = mbedtls_md_setup(&shaCtx,
                       mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                       1);
  if (i != 0) {
    // Clear AES and SHA contexts to exit
    mbedtls_aes_free(&aesCtx);
    mbedtls_md_free(&shaCtx);
    mbedtls_printf("  ! mbedtls_md_setup() returned -0x%04x\n", -i);
    return;
  }

  //  The encrypted file must be structured as follows:
  //        00 .. 15              Initialization Vector
  //        16 .. 31              AES Encrypted Block #1
  //           ..
  //      N*16 .. (N+1)*16 - 1    AES Encrypted Block #N
  //  (N+1)*16 .. (N+1)*16 + 32   HMAC-SHA-256(ciphertext)

  if (messageSize < (2 * IV_SIZE + 2 * TAG_SIZE)) {
    // Clear AES and SHA contexts to exit
    mbedtls_aes_free(&aesCtx);
    mbedtls_md_free(&shaCtx);
    mbedtls_printf("File too short to be encrypted.\n");
    return;
  }

  if ((messageSize & 0x0F) != 0) {
    // Clear AES and SHA contexts to exit
    mbedtls_aes_free(&aesCtx);
    mbedtls_md_free(&shaCtx);
    mbedtls_printf("File size not a multiple of 16.\n");
    return;
  }

  // Subtract the IV + HMAC length.
  messageSize -= (2 * IV_SIZE + 2 * TAG_SIZE);

  // Read the initial vector (IV).
  hexToBin((uint8_t *)initialVector, IV_SIZE, message);

  // The digest[0..15] = IV[0..15] and digest[16..31] = 0
  memset((digest + IV_SIZE), 0, IV_SIZE);
  memcpy(digest, initialVector, IV_SIZE);

  // Convert ASCII hard-coded key into binary
  hexToBin((uint8_t *)buffer, KEY_SIZE, aesKey256);

  DWT->CYCCNT = 0;
  // Hash the IV and the secret key together 8192 times
  for (i = 0; i < 8192; i++) {
    mbedtls_md_starts(&shaCtx);
    mbedtls_md_update(&shaCtx, digest, TAG_SIZE);
    mbedtls_md_update(&shaCtx, buffer, KEY_SIZE);
    mbedtls_md_finish(&shaCtx, digest);
  }

  // Using the result in digest as key to setup the AES and HMAC.
  mbedtls_aes_setkey_dec(&aesCtx, digest, KEY_SIZE * 8);
  mbedtls_md_hmac_starts(&shaCtx, digest, TAG_SIZE);

  // Set null character to end of plain text for print out
  buffer[AES_BLOCK_SIZE] = 0;

  // Decrypt ciphertext and write the plain text.
  for (bufPtr = &message[2 * IV_SIZE], offset = 0;
       offset < messageSize;
       offset += 2 * AES_BLOCK_SIZE, bufPtr += 2 * AES_BLOCK_SIZE ) {
    hexToBin(buffer, AES_BLOCK_SIZE, bufPtr);
    // Copy the binary ciphertext to digest
    memcpy(digest, buffer, AES_BLOCK_SIZE);

    // HMAC then decrypt the ciphertext
    mbedtls_md_hmac_update(&shaCtx, buffer, AES_BLOCK_SIZE);
    mbedtls_aes_crypt_ecb(&aesCtx, MBEDTLS_AES_DECRYPT, buffer, buffer);

    // XOR IV to get plain text
    for (i = 0; i < AES_BLOCK_SIZE; i++) {
      buffer[i] = (unsigned char)(buffer[i] ^ initialVector[i]);
    }

    // Restore the ciphertext to XOR the following decrypted text
    memcpy(initialVector, digest, AES_BLOCK_SIZE);

    // Print out plain text
    mbedtls_printf((char*) buffer);
  }

  // Finally write the HMAC.
  mbedtls_md_hmac_finish(&shaCtx, digest);
  cycles = DWT->CYCCNT;

  // Binary digest to buffer
  hexToBin(buffer, TAG_SIZE, bufPtr);

  // Verify the message authentication code.
  offset = 0;
  for (i = 0; i < TAG_SIZE; i++) {
    offset |= digest[i] ^ buffer[i];
  }

  if (offset != 0) {
    mbedtls_printf("\nHMAC check failed: wrong key, "
                   "or file corrupted.\n");
  } else {
    mbedtls_printf("\nMessage digest tag OK.\n");
    mbedtls_printf("Decryption, cycles: %" PRIu32 " time: %" PRIu32 " ms\n",
                   cycles,
                   cycles / (CMU_ClockFreqGet(cmuClock_HCLK) / 1000));
  }

  // Clear AES and SHA contexts to exit
  mbedtls_aes_free(&aesCtx);
  mbedtls_md_free(&shaCtx);
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

  while (1) {
    // Print out welcome text
    mbedtls_printf("\nWelcome to AESCRYPT. ");
    mbedtls_printf("Core running at %" PRIu32 " kHz.\n",
                   CMU_ClockFreqGet(cmuClock_HCLK) / 1000);
    mbedtls_printf("Please type 'e' to encrypt or 'd' to decrypt: ");

    // Wait input to run encryption or decryption
    if (selectAesMode() == MODE_ENCRYPT) {
      aesEncrypt();
    } else {
      aesDecrypt();
    }
  }
}
