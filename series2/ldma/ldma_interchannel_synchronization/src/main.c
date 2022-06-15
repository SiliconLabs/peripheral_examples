/***************************************************************************//**
 * @file main.c
 * @brief This example demonstrates the LDMA inter-channel synchronization. See
 * readme.txt for details.
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

#include "em_chip.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "em_prs.h"
#include "em_cmu.h"
#include "bsp.h"

// Constants for inter-channel sync transfer
#define SYNC_SET            0x80
#define SYNC_CLEAR          0x80
#define MATCH_VALUE         0x80

// Channels used for the example
#define LDMA_CHANNEL        0
#define GPIO_PRS_CHANNEL    1

// Memory to memory transfer buffer size
#define BUFFER_SIZE         4

// Descriptor linked lists for LDMA transfer
LDMA_Descriptor_t descLink0[3];
LDMA_Descriptor_t descLink1[2];

// Buffers for memory to memory transfer
uint8_t srcA[BUFFER_SIZE] = "AAaa";
uint8_t srcC[BUFFER_SIZE] = "CCcc";
uint8_t srcY[BUFFER_SIZE] = "YYyy";

uint8_t dstBuffer[BUFFER_SIZE];

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler(void)
{
  uint32_t pending;

  // Read interrupt source
  pending = LDMA_IntGet();

  // Clear interrupts
  LDMA_IntClear(pending);

  // Check for LDMA error
  if (pending & LDMA_IF_ERROR){
    // Loop here to enable the debugger to see what has happened
    while (1);
  }
}
/***************************************************************************//**
 * @brief
 *   Clock Initialization.
 ******************************************************************************/
static void initCMU(void)
{
  // Initialize peripheral clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_LDMA, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  /* Note: For EFR32xG21 radio devices, library function calls to
   * CMU_ClockEnable() have no effect as oscillators are automatically turned
   * on/off based on demand from the peripherals; CMU_ClockEnable() is a dummy
   * function for EFR32xG21 for library consistency/compatibility.
   */

}

/***************************************************************************//**
 * @brief
 *   Setup push button PB0 and PB1 as PRS source for DMAREQ0 and DMAREQ1.
 ******************************************************************************/
static void gpioPrsSetup(void)
{
  // Configure push buttons as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN,
                  gpioModeInputPullFilter, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN,
                  gpioModeInputPullFilter, 1);

  // Configure PRS sensing on push buttons
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, false, false, false);
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN, false, false, false);

  // Select GPIO as PRS source and push buttons as signals for PRS channels
  PRS_SourceAsyncSignalSet(GPIO_PRS_CHANNEL, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           BSP_GPIO_PB0_PIN);
  PRS_SourceAsyncSignalSet(GPIO_PRS_CHANNEL+1, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           BSP_GPIO_PB1_PIN);

  // DMA request is high active, invert PRS signals to default low
  PRS_Combine(GPIO_PRS_CHANNEL, GPIO_PRS_CHANNEL, prsLogic_NOT_A);
  PRS_Combine(GPIO_PRS_CHANNEL+1, GPIO_PRS_CHANNEL+1, prsLogic_NOT_A);

  // Select PRS channels for DMA request 0 and 1
  PRS_ConnectConsumer(GPIO_PRS_CHANNEL, prsTypeAsync, prsConsumerLDMA_REQUEST0);
  PRS_ConnectConsumer(GPIO_PRS_CHANNEL+1, prsTypeAsync,
                      prsConsumerLDMA_REQUEST1);
}

/***************************************************************************//**
 * @brief
 *   Initialize the LDMA controller for inter-channel synchronization
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  // Initialize buffers for memory transfer
  for (i = 0; i < BUFFER_SIZE; i++)
  {
    dstBuffer[i] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Use peripheral transfer configuration macro for DMA channels
  LDMA_TransferCfg_t periTransferPB0 = 
    LDMA_TRANSFER_CFG_PERIPHERAL(LDMAXBAR_CH_REQSEL_SIGSEL_LDMAXBARPRSREQ0
                                 | LDMAXBAR_CH_REQSEL_SOURCESEL_LDMAXBAR);
  LDMA_TransferCfg_t periTransferPB1 =
    LDMA_TRANSFER_CFG_PERIPHERAL(LDMAXBAR_CH_REQSEL_SIGSEL_LDMAXBARPRSREQ1
                                 | LDMAXBAR_CH_REQSEL_SOURCESEL_LDMAXBAR);
  
  // Set up descriptor links
  // Transfer AAaa
  descLink0[0] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_M2M_BYTE (&srcA, &dstBuffer, BUFFER_SIZE, 1);
  // Wait for MATCH_VALUE to be set
  descLink0[1] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0, MATCH_VALUE, MATCH_VALUE, 1);
  // Transfer CCcc
  descLink0[2] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_SINGLE_M2M_BYTE(&srcC, &dstBuffer, BUFFER_SIZE);

  // Transfer YYyy
  descLink1[0] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(&srcY, &dstBuffer, BUFFER_SIZE, 1);
  // Set SYNC_SET value
  descLink1[1] = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_SINGLE_SYNC(SYNC_SET, 0, 0, 0);

  // First transfer in each set waits for request
  descLink0[0].xfer.structReq = false;
  descLink1[0].xfer.structReq = false;

  // Start both channels
  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferPB0, (void*)&descLink0);
  LDMA_StartTransfer(LDMA_CHANNEL+1, (void*)&periTransferPB1,
                     (void*)&descLink1);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator if available
#if defined( DCDC )  
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
#endif
  
  // Initialize clock
  initCMU();

  // Initialize GPIO for PRS
  gpioPrsSetup();

  // Initialize LDMA
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
