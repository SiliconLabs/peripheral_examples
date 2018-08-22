/**************************************************************************//**
 * @main_series0.c
 * @brief This project uses the CTR (Counter) mode of AES
 * encryption to encrypt the user's input data and then decrypt it.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_aes.h"

// Note: change this to change the number of bytes to encrypt
//       (must be a multiple of 16)
#define DATA_SIZE 64

// Note: change this to change the data to encrypt
const static uint8_t originalData[DATA_SIZE] = {
  0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
  0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
  0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C,
  0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51,
  0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11,
  0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF,
  0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17,
  0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10,
};

// A buffer to hold the encrypted data
static uint8_t encryptedData[DATA_SIZE];

// A buffer to hold the decrypted data
static uint8_t decryptedData[DATA_SIZE];

// Number of bytes to make up a 128 bit key (16 * 8 = 128)
#define KEY_SIZE 16

// Note: change this to change the key
// Note: must be 128 bits
// Note: the same key is used for encryption and decryption in CTR mode
const static uint8_t key[KEY_SIZE] = {
  0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
  0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

// Number of bytes to make up a 128 bit counter (16 * 8 = 128)
#define COUNTER_SIZE 16

// Note: change this to change the initial counter
// Note: must be 128 bits
const uint8_t initialCounter[COUNTER_SIZE] = {
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
  0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

// A buffer to hold the counter value that will be updated after each AES block encoding
uint8_t counterBuffer[COUNTER_SIZE];

// A flag indicating whether the encryption/decryption process succeeded
// Note: This is only volatile to ensure that it doesn't get optimized out by
// the compiler before the user checks its value. It's not actually necessary.
static volatile bool isError;

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Enable AES clock
  CMU_ClockEnable(cmuClock_AES, true);

  // Copy the initialCounter into the counterBuffer
  for (uint32_t i = 0; i < COUNTER_SIZE; i++) {
    counterBuffer[i] = initialCounter[i];
  }

  // Encrypt data using AES-128 CTR
  // Note: the encryption and decryption operations are identical in CTR mode
  AES_CTR128(encryptedData,       // Pointer to buffer where encrypted/decrypted data will be put
             originalData,        // Pointer to buffer holding data to encrypt/decrypt
             DATA_SIZE,           // Number of bytes to encrypt (must be a multiple of 16)
             key,                 // A 128 bit encryption/decryption key
             counterBuffer,       // Pointer to buffer with initial counter value
             AES_CTRUpdate32Bit); // Pointer to a function that will update the counter value

  // Reinitialize the counterBuffer for decryption
  for (uint32_t i = 0; i < COUNTER_SIZE; i++) {
    counterBuffer[i] = initialCounter[i];
  }

  // Decrypt data using AES-128 CTR
  // Note: the encryption and decryption operations are identical in CTR mode
  AES_CTR128(decryptedData,       // Pointer to buffer where encrypted/decrypted data will be put
             encryptedData,       // Pointer to buffer holding data to encrypt/decrypt
             DATA_SIZE,           // Number of bytes to encrypt (must be a multiple of 16)
             key,                 // A 128 bit encryption/decryption key
             counterBuffer,       // Pointer to buffer with initial counter value
             AES_CTRUpdate32Bit); // Pointer to a function that will update the counter value

  // Check whether decrypted result is identical to the original data
  isError = false;
  for (uint32_t i = 0; i < DATA_SIZE; i++) {
    if (decryptedData[i] != originalData[i]) {
      isError = true;
    }
  }

  // Pause the debugger here to check if the isError variable is true/false
  while (1) {
    EMU_EnterEM1();
  }
}

