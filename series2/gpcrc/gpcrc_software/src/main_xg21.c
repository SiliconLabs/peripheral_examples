/***************************************************************************//**
 * @file main.c
 * @brief Use the GPCRC to check data
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

