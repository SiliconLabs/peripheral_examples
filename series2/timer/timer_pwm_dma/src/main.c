/***************************************************************************//**
 * @file main.c
 *
 * @brief This project demonstrates DMA-driven pulse width modulation
 * using the TIMER module.  GPIO pin PA6 is configured to  output a
 * 1 kHz signal.  The DMA continuously updates the OCB register to vary
 * the duty cycle.
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

//  Desired frequency in Hz
#define PWM_FREQ 1000

/*
 * This table holds the time calculated for each given duty cycle value
 * expressed as a percent.  Note that BUFFER_SIZE must match the number
 * of values in dutyCyclePercentages[BUFFER_SIZE].
 */
#define BUFFER_SIZE 11
static uint32_t buffer[BUFFER_SIZE];

// Each change in duty cycle expressed as a percent
static const uint32_t dutyCyclePercentages[BUFFER_SIZE] =
    {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

// Globally declared link descriptor
LDMA_Descriptor_t descLink;

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure PA6 as Push-Pull
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);
}

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

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  uint32_t timerFreq, topValue;
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  // Don't start counter on initialization
  timerInit.enable = false;

  // PWM mode sets/clears the output on compare/overflow events
  timerCCInit.mode = timerCCModePWM;

  TIMER_Init(TIMER0, &timerInit);

  // Route CC0 output to PA6
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortA << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (6 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set top value to overflow at the desired PWM_FREQ frequency
  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);
  topValue = (timerFreq / PWM_FREQ);
  TIMER_TopSet(TIMER0, topValue);

  // Now start the TIMER
  TIMER_Enable(TIMER0, true);

  // Trigger DMA on compare event to set CCVB to update duty cycle on next period
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
}

/**************************************************************************//**
 * @brief
 *    Populate buffer with calculated duty cycle time values
 *****************************************************************************/
void populateBuffer(void)
{
  uint32_t i, topVal;

  // 100% duty cycle is the maximum count value
  topVal = TIMER_TopGet(TIMER0);

  for (i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = (topVal * dutyCyclePercentages[i]) / 100;
  }
}

/**************************************************************************//**
* @brief
*    Initialize the LDMA module
*
* @details
*    Configure the channel descriptor to use the default peripheral to
*    memory transfer descriptor. Modify it to not generate an interrupt
*    upon transfer completion (we don't need it). Additionally, the transfer
*    configuration selects the TIMER0_CC0 signal as the trigger for the LDMA
*    transaction to occur.
*
* @note
*    The descriptor object needs to at least have static scope persistence so
*    that the reference to the object is valid beyond its first use in
*    initialization
*****************************************************************************/
void initLDMA(void)
{
  // Default LDMA initialization
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Request transfers on CC0 peripheral requests
  LDMA_TransferCfg_t periTransferTx =
          LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_TIMER0_CC0);

  // Transfer from the RAM buffer to CC0 output register in a continuous loop
  LDMA_Descriptor_t xfer =
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&buffer,             // Memory source address
                                     &TIMER0->CC[0].OCB,  // Output compare buffer register
                                     BUFFER_SIZE,         // Number of transfers to make
                                     0);                  // Link to same descriptor

  // Store descriptor globally
  descLink = xfer;

  // Transfer one word per request
  descLink.xfer.size = ldmaCtrlSizeWord;

  // Do not ignore single requests.  Transfer data on every request.
  descLink.xfer.ignoreSrec = 0;

  // Do not request an interrupt on completion of all transfers
  descLink.xfer.doneIfs  = 0;

  // Start transfer, LDMA will trigger after first compare event
  LDMA_StartTransfer(0, (void*)&periTransferTx, (void*)&descLink);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  initCMU();
  initGPIO();
  initTIMER();

  // Initialize DMA only after buffer is populated
  populateBuffer();
  initLDMA();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}
