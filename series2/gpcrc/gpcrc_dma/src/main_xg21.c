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
#include "em_ldma.h"
#include "em_emu.h"

#define PRESET      0xFFFFFFFFUL
/*
 * This data was obtained from this online CRC calculator - https://crccalc.com/
 * The CRC is calculated with the input data shown below -
 * 0x00000001, 0x00000001, 0x00000002, 0x00000003, 0x00000005, 0x00000008
 * 0x0000000D, 0x00000015, 0x00000022, 0x00000037, 0x00000059, 0x00000090,
 * 0x000000E9, 0x00000179, 0x00000262, 0x000003DB
 *
 * However, the data is stored least significant byte first in RAM, which will
 * change the input data to the GPCRC. Therefore, the input to the GPCRC is byte
 * reversed so that CRC is calculated for the input shown above instead of
 * 0x01000000, ...., 0xDB030000.
 *
 * As a part of post processing in the crc calculator shown above, the resulting
 * CRC is XOR'd with 0xFFFFFFFF. This is not done by the GPCRC module on the
 * device. Therefore, the CRC of the data is XOR'd with 0xFFFFFFFF before
 * comparing it with the result of the online calculator.
 */
// IEEE 802.3 CRC for the input fibonacci data
#define FIBONACCI_CRC_16WORDS (0x5CBF42AAUL)


// DMA channel used
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        1 << LDMA_CHANNEL

// Descriptor linked list for LDMA transfer
LDMA_Descriptor_t ldmaCrcDesc[2];

// store 16 words (16 x 4 = 64 bytes) in RAM
#define ARRAY_SIZE 16

// Arbitrary values to cycle through
uint32_t data[ARRAY_SIZE];

// LDMA GPCRC channel assignment
#define LDMA_GPCRC_CHAN  0

// Register bit clear address for LDMA_IEN
#define LDMA_IEN_CLEAR  (((uint32_t)&(LDMA->IEN)) + 0x04000000)

/**************************************************************************//**
 * @brief  Calculates the n-th Fibonacci number recursively.
 *****************************************************************************/
uint32_t fib(uint32_t n)
{
  if (n < 2)
  {
    return 1;
  }
  return (fib(n-1) + fib(n-2));
}

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler( void )
{
  // Shouldn't ever get in here, so take a breakpoint
  __BKPT(0);

}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for descriptor list with looping
 ******************************************************************************/
void initLdma(void)
{
  /*
   * LDMA module initialization.  Note that the LDMA_IF_ERROR interrupt
   * is also enabled by LDMA_Init(), so it's effectively mandatory to
   * have LDMA_IRQHandler() because of this just to catch the error
   * interrupt, if it happens.
   */
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );
}

/**************************************************************************//**
 * @brief  GPCRC Initializer
 *****************************************************************************/
void initGpcrc (void)
{
  // GPCRC module initialization for IEEE 802.3 polynomial
  GPCRC_Init_TypeDef init = GPCRC_INIT_DEFAULT;
  // Starting value in GPCRC_DATA
  init.initValue = PRESET;
  // Reset GPCRC_DATA to 0xFFFF_FFFF after every read
  init.autoInit = true;
  // Reverse all bytes of the incoming message
  init.reverseByteOrder = true;

  // Initialize GPCRC
  GPCRC_Init(GPCRC, &init);
}

/**************************************************************************//**
 * @brief  Uses LDMA to transfer the array data into GPCRC->INPUTDATA register
 *****************************************************************************/
void crcCheckStart (uint32_t *inputData)
{
  // Prep the GPCRC
  GPCRC_Start(GPCRC);

  // Use the generic memory-to-memory transfer configuration; halt during debug
  LDMA_TransferCfg_t transferConfig = LDMA_TRANSFER_CFG_MEMORY();
  transferConfig.ldmaDbgHalt = true;

  /*
   * Even though the GPCRC is a peripheral, it's necessary to use the
   * M2M descriptor because there is no DMA request (such as RXDATAV
   * upon USART reception).  Instead, the LDMA simply needs to pump
   * the page of flash through the GPCRC, word by word, as fast as it
   * can manage to do so.
   *
   * One minor inconvenience here is that even when the descriptor's
   * doneIfs field is set to 0 in order to avoid an interrupt upon
   * channel completion, the LDMA always sets the corresponding
   * LDMA_IF flag after executing the last descriptor.
   *
   * Setting the LDMA_IF flag in and of itself isn't a problem save
   * for the fact that LDMA_StartTransfer() enables a channel's
   * interrupt no matter what.
   *
   * Because the CRC blank checking operation is intended to run in
   * this example until software checks to see if it is actually
   * done, a little bit of trickery is used to disable the interrupt
   * right after LDMA_StartTransfer() enables it.
   *
   * Instead of using a single descriptor to execute the GPCRC
   * operation, a two element linked list is used.  The first entry
   * in the list is a WRI descriptor that writes the bit mask for the
   * GPCRC LDMA channel to the LDMA_IEN register in the peripheral
   * bit clear aliasing region (register address + 0x04000000).  The
   * next descriptor is the M2M transfer that feeds the flash page
   * contents into the GPCRC_INPUTDATA register word by word.
   */

  ldmaCrcDesc[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE((1 << LDMA_GPCRC_CHAN), &(LDMA -> IEN_CLR), 1);
  ldmaCrcDesc[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2M_WORD(inputData, &(GPCRC->INPUTDATA), ARRAY_SIZE);
  ldmaCrcDesc[1].xfer.dstInc = ldmaCtrlDstIncNone;


  // This starts the CRC calculation.
  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&transferConfig, (void*)ldmaCrcDesc);
}

/**************************************************************************//**
 * @brief  Check to wait till LDMA transfer is completed
 *****************************************************************************/
bool crcCheckTransferBusy(void)
{
  if (LDMA_TransferDone(LDMA_GPCRC_CHAN) == true)
    return false;
  else
    return true;
}

/**************************************************************************//**
 * @brief  Reads GPCRC DATA register and compares with IEEE 802.3 result
 *****************************************************************************/
bool crcCheckResult(void)
{
  // XOR the data register output with 0xFFFFFFFF to get the post-processed
  // IEEE 802.3 result
  uint32_t crcResult = GPCRC_DataRead(GPCRC)^PRESET;

  if (crcResult == FIBONACCI_CRC_16WORDS)
    return true;
  else
    return false;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  bool crcCheck = false;

  // Fill data array with fibonacci values
  for (uint32_t i = 0; i < ARRAY_SIZE; i++){
      data[i] = (fib(i));
  }

  // Initialize GPCRC
  initGpcrc();

  // Initialize LDMA
  initLdma();

  // Transfer the data using the LDMA to the GPCRC input
  crcCheckStart(data);

  // Wait LDMA transfer is in progress
  while (crcCheckTransferBusy() == true);

  // Read the GPCRC output and compare with predetermined CRC
  crcCheck = crcCheckResult();

  if (!crcCheck){
      // Halt if crcCheck fails
      __BKPT(2);
  }

  // Infinite loop
  while(1){
      // Enter EM1 if CRC check passes
      EMU_EnterEM1();
  }
}
