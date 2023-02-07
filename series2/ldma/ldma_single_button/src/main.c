/***************************************************************************//**
 * @file main.c
 * @brief This example demonstrates the single direct register LDMA transfer.
 * See readme.txt for details.
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
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "em_prs.h"
#include "bsp.h"

// DMA channel used for the example
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        (1 << LDMA_CHANNEL)

// Memory to memory transfer buffer size and constants for GPIO PRS
#define BUFFER_SIZE         128
#define TRANSFER_SIZE       BUFFER_SIZE
#define USE_GPIO_PRS        0
#define GPIO_PRS_CHANNEL    1

// Buffers for memory to memory transfer
uint16_t srcBuffer[BUFFER_SIZE];
uint16_t dstBuffer[BUFFER_SIZE];

LDMA_Descriptor_t des;

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
  if (pending & LDMA_IF_ERROR)
  {
    // Loop here to enable the debugger to see what has happened
    while (1);
  }
}

#if (USE_GPIO_PRS == 1)
/***************************************************************************//**
 * @brief
 *   Setup push button PB1 as PRS source for DMAREQ0.
 ******************************************************************************/
static void gpioPrsSetup(void)
{
  // Enable peripheral clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);
  /* Note: For EFR32xG21 radio devices, library function calls to
   * CMU_ClockEnable() have no effect as oscillators are automatically turned
   * on/off based on demand from the peripherals; CMU_ClockEnable() is a dummy
   * function for EFR32xG21 for library consistency/compatibility.
   */

  // Configure push button PB1 as input
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN,
                  gpioModeInputPullFilter, 1);

  // Configure PRS sensing on push button PB1
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN,
					false, false, false);

  // Select GPIO as PRS source and push button PB1 as signal for PRS channel
  PRS_SourceAsyncSignalSet(GPIO_PRS_CHANNEL, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           BSP_GPIO_PB1_PIN);

  // DMA request is high active, invert PRS signal to default low
  PRS_Combine(GPIO_PRS_CHANNEL, GPIO_PRS_CHANNEL, prsLogic_NOT_A);

  // Select PRS channel for DMA request 0
  PRS_ConnectConsumer(GPIO_PRS_CHANNEL, prsTypeAsync, prsConsumerLDMA_REQUEST0);
}
#endif

/***************************************************************************//**
 * @brief
 *   Setup LDMA to be triggered either by software or by pressing PB1
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  // Enable peripheral clock
  CMU_ClockEnable(cmuClock_LDMA, true);
  /* Note: For EFR32xG21 radio devices, library function calls to
   * CMU_ClockEnable() have no effect as oscillators are automatically turned
   * on/off based on demand from the peripherals; CMU_ClockEnable() is a dummy
   * function for EFR32xG21 for library consistency/compatibility.
   */


  // Initialize buffers for memory transfer
  for (i = 0; i < BUFFER_SIZE; i++)
  {
    srcBuffer[i] = i;
    dstBuffer[i] = 0;
  }

  // Initialize LDMA with default settings
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  #if (USE_GPIO_PRS==1)
  // Use peripheral transfer configuration macro for DMA channels
  LDMA_TransferCfg_t periTransferPB1 =
    LDMA_TRANSFER_CFG_PERIPHERAL(LDMAXBAR_CH_REQSEL_SIGSEL_LDMAXBARPRSREQ0
                                 | LDMAXBAR_CH_REQSEL_SOURCESEL_LDMAXBAR);
#else
  LDMA_TransferCfg_t periTransferPB1 =
    LDMA_TRANSFER_CFG_MEMORY();
#endif

  // Set up descriptor links
  des = (LDMA_Descriptor_t)
    LDMA_DESCRIPTOR_SINGLE_M2M_WORD(&srcBuffer, &dstBuffer, TRANSFER_SIZE);

  // Transfer half word size
  des.xfer.size = ldmaCtrlSizeHalf;
  des.xfer.reqMode = ldmaCtrlReqModeAll;
  des.xfer.structReq = false;

  // Start both channels
  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferPB1, (void*)&des);

#if (USE_GPIO_PRS==0)
  LDMA->SWREQ |= LDMA_CH_MASK;
#endif
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
  
#if (USE_GPIO_PRS == 1)
  // Initialize GPIO for PRS
  gpioPrsSetup();
#endif
  
  // Initialize LDMA
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
