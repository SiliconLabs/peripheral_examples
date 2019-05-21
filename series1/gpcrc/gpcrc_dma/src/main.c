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
#include "em_ldma.h"
#include "em_emu.h"


/* The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard. */
typedef uint32_t crc_t;


#define WIDTH       (8 * sizeof(crc_t))
#define TOPBIT      (1 << (WIDTH - 1))
#define POLYNOMIAL  0x04C11DB7
#define PRESET      0xFFFFFFFF

// DMA channel used
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        1 << LDMA_CHANNEL

// Descriptor linked list for LDMA transfer
LDMA_Descriptor_t descLink[2];

// Arbitrary values to cycle through
uint32_t data[4] = {0, 1, 2, 3};

volatile uint32_t   source;
volatile uint32_t   result;
volatile uint32_t   i;

volatile crc_t      crcTable[256];


/***************************************************************************//**
 * @brief
 *   Initialize software ctcTable array for fast translations.
 ******************************************************************************/
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

/***************************************************************************//**
 * @brief
 *   Software CRC calculation function
 ******************************************************************************/
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

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler( void )
{
  uint32_t pending;

  // Read interrupt source
  pending = LDMA_IntGet();

  // Check for LDMA error
  if ( pending & LDMA_IF_ERROR )
  {
    // Loop here to enable the debugger to see what has happened
    while (1);
  }

  // Clear interrupts
  LDMA_IntClear(pending);

  source = data[(++i)%4];

  // Request next transfer
  LDMA->SWREQ |= LDMA_CH_MASK;

}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for descriptor list with looping
 ******************************************************************************/
void initLdma(void)
{

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );

  // Use looped peripheral transfer configuration macro
  LDMA_TransferCfg_t transferConfig = LDMA_TRANSFER_CFG_MEMORY();

  // LINK descriptor macros for looping, SINGLE descriptor macro for single transfer
  // See README for explanation of why a "M2M" (Memory to memory) transfer is used here instead
  // of a M2P (Memory to Peripheral) or P2M (Peripheral to Memory)
  descLink[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_WORD(&source, &(GPCRC->INPUTDATA), 1, 1);
  descLink[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_WORD(&(GPCRC->DATA), &result, 1, -1);

  // Enable interrupt after second descriptor
  descLink[1].xfer.doneIfs = true;

  // Disable automatic triggers for first descriptor
  descLink[0].xfer.structReq = false;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&transferConfig, (void*)&descLink);
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

  // Starting value in GPCRC_DATA
  init.initValue = PRESET;

  // Reset GPCRC_DATA to 0xFFFF_FFFF after every read
  init.autoInit = true;

  // Initialize GPCRC
  GPCRC_Init(GPCRC, &init);

  // Reset out GPCRC_DATA
  GPCRC_Start(GPCRC);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Initialize GPCRC
  initGpcrc();

  // Initialize LDMA
  initLdma();

  // Initialize array for software method
  initSoft();

  // Set up global variables
  i = 0;
  source = data[i++];

  // Request first transfer
  LDMA->SWREQ |= LDMA_CH_MASK;

  // Infinite loop
  while(1){
    EMU_EnterEM1();
  }
}
