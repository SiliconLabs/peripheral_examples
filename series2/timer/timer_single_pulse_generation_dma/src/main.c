/***************************************************************************//**
 * @file main.c
 (
 * @brief This project demonstrates the generation of a single pulse using
 * output compare and the LDMA. Refer to the readme.txt file for testing
 * instructions.
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "em_timer.h"

/*
 * Change this to modify the length of the delay from when the TIMER
 * starts counting to when CC0 drives the GPIO pin high.
 */
#define NUM_SECONDS_DELAY  3

// Compare count for first edge (assumes 19 MHz HFRCO)
#define COMPARE_VALUE1     (NUM_SECONDS_DELAY * 19000000)

// Length of the pulse high time in milliseconds
#define PULSE_WIDTH        1

// Compare count for second edge
#define COMPARE_VALUE2     (COMPARE_VALUE1 + (PULSE_WIDTH * 19000000 / 1000))

// Globally stored value for DMA transfer
uint32_t value = COMPARE_VALUE2;

// Globally declared link descriptor
LDMA_Descriptor_t descLink;

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCMU(void)
{
  /*
   * Enable the GPIO and TIMER0 bus clocks.
   *
   * Note: On EFR32xG21 devices, calls to CMU_ClockEnable() have no
   * effect as clocks are automatically turned on/off in response to
   * on-demand requests from the peripherals.  CMU_ClockEnable() is
   * a dummy function on EFR32xG21 present for software compatibility.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
}

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
  if (pending & LDMA_IF_ERROR) {
    // Loop here so the debugger can be stopped to see what has happened
    while (1);
  }
}

/**************************************************************************//**
 * @brief Configure and start DMA
 *****************************************************************************/
void initLDMA(void)
{
  // Default LDMA initialization
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Request transfers on CC0 peripheral requests
  LDMA_TransferCfg_t periTransferTx =
          LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_TIMER0_CC0);

  // Get default descriptor for transfer from TIM0_CC0 to the buffer
  LDMA_Descriptor_t xfer =
    LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(&value,            // Memory source address
                                    &TIMER0->CC[0].OC, // Output compare register
                                    1);                // Just 1 transfer

  // Store descriptor globally
  descLink = xfer;

  // Transfer one word per unit
  descLink.xfer.size = ldmaCtrlSizeWord;

  // Do not ignore single requests
  // IE: When Timer CC0 event occurs, transfer 1 unit then wait for next compare
  descLink.xfer.ignoreSrec = 0;

  // Start transfer, LDMA will trigger after first compare event
  LDMA_StartTransfer(0, (void*)&periTransferTx, (void*)&descLink);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  // Do not start counter upon initialization
  timerInit.enable = false;

  // Set drive output pin to toggle upon compare match
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle;

  TIMER_Init(TIMER0, &timerInit);

  // Route CC0 output to PA6
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortA << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (6 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  /*
   * Overwrite the default of 0xFFFF in TIMER_TOP with 0xFFFFFFFF
   * because TIMER0 is 32 bits wide.
   */
  TIMER_TopSet(TIMER0, 0xFFFFFFFF);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set the count for the first output compare to match
  TIMER_CompareSet(TIMER0, 0, COMPARE_VALUE1);

  // Now start the TIMER
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  initCMU();

  // Configure PA6 as an output
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);

  initLDMA();

  initTIMER();

  while (1)
  {
    EMU_EnterEM1();
  }
}
