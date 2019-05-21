/**************************************************************************//**
 * @file
 * @brief Use the GPCRC to check data
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpcrc.h"

/* The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard. */
typedef uint32_t crc_t;

#define WIDTH       (8 * sizeof(crc_t))
#define TOPBIT      (1 << (WIDTH - 1))
#define POLYNOMIAL  0x04C11DB7
#define PRESET      0xFFFFFFFF

#define ARRAY_SIZE  16
#define STRIDE      0xABCD

volatile crc_t      crcTable[256];

volatile uint32_t   source[ARRAY_SIZE];
volatile uint32_t   results[ARRAY_SIZE];
volatile uint32_t   softResults[ARRAY_SIZE];


void initSoft(void)
{
  crc_t  remainder;

  // Compute the remainder of each possible dividend
  for (int i = 0; i < 256; ++i)
  {
    // Start with the dividend followed by zeros
    remainder = i << (WIDTH - 8);

    // Perform modulo-2 division, a bit at a time
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      // XOR with polynomial if top bit is 1
      if (remainder & (uint32_t)TOPBIT)
        remainder = (remainder << 1) ^ POLYNOMIAL;
      else
        remainder = (remainder << 1);
    }
    // Store the result into the table
    crcTable[i] = remainder;
  }
}

crc_t softCrc(crc_t message, crc_t preset)
{
  uint8_t data, thisByte;
  crc_t remainder = preset;

  // Divide the message by the polynomial, a byte at a time
  for (int byte = 0; byte < sizeof(crc_t); ++byte)
  {
    // Separate out one byte of the message
    thisByte = (message >> (8*byte)) & 0x0FF;

    // XOR this byte with the current remainder
    data = thisByte ^ (remainder >> (WIDTH - 8));

    // Set new remainder
    remainder = crcTable[data] ^ (remainder << 8);
  }
  // The final remainder is the CRC
  return (remainder);
}

/**************************************************************************//**
 * @brief  GPCRC Initializer
 *****************************************************************************/
void initGpcrc (void)
{
  // Enable clocks required
  CMU_ClockEnable(cmuClock_GPCRC, true);

  // Declare init structs
  GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;

  init.initValue = PRESET;  // Starting value in GPCRC_DATA
  init.autoInit = true;     // Reset GPCRC_DATA to 0xFFFF_FFFF after every read
  init.reverseBits = true;  // Reverse all bits of the incoming message

  // Initialize GPCRC
  GPCRC_Init(GPCRC, &init);

  // Prepare GPCRC_DATA for inputs
  GPCRC_Start(GPCRC);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Fill source array with arbitrary values
  for (int i = 0; i < ARRAY_SIZE; i++){
    source[i] = (1 + i) * STRIDE;
  }

  // Initialize GPCRC
  initGpcrc();
  // Initialize software CRC
  initSoft();

  // Calculate CRC values for every value in source array
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    // Get software CRC result
    softResults[i] = softCrc(source[i], PRESET);

    // Feed in source
    GPCRC_InputU32(GPCRC, source[i]);

    // Read result
    results[i] = GPCRC_DataReadBitReversed(GPCRC);
  }

  // Infinite loop
  while(1);
}

