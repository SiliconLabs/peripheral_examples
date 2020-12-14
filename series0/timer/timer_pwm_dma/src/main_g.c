/***************************************************************************//**
 * @file main_g.c
 * @brief This project demonstrates DMA driven pulse width modulation using the
 * TIMER module. The GPIO pin specified in the readme.txt is configured to output
 * a 1kHz signal. The DMA continuously updates the CCVB register to vary the
 * duty cycle.
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"

// Note: change this to set the desired Output frequency in Hz
#define PWM_FREQ 1000

// Buffer size
#define BUFFER_SIZE 11

// Note: change this to change the duty cycles used in this example
static const uint16_t dutyCyclePercentages[BUFFER_SIZE] =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

// Buffer of duty cycle values for DMA transfer to CCVB
// Buffer is populated after TIMER is initialized and Top value is set
static uint16_t buffer[BUFFER_SIZE];

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD1 as output
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure TIMER0 Compare/Capture for output compare
  // Use PWM mode, which sets output on overflow and clears on compare events
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Route TIMER0 CC0 to location 3 and enable CC0 route pin
  // TIM0_CC0 #3 is GPIO Pin PD1
  TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC3);

  // Set top value to overflow at the desired PWM_FREQ frequency
  TIMER_TopSet(TIMER0, CMU_ClockFreqGet(cmuClock_TIMER0) / PWM_FREQ);

  // Initialize and start timer with no prescaling
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.dmaClrAct = true;

  TIMER_Init(TIMER0, &timerInit);
}

/**************************************************************************//**
 * @brief
 *    Populate buffer with timer duty cycle values
 *****************************************************************************/
void populateBuffer(void)
{
  for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = (uint16_t) (TIMER_TopGet(TIMER0) * dutyCyclePercentages[i] / 100);
  }
}

/***************************************************************************//**
* @brief
*   DMA callback function
*
* @details
*   This function gets called after the DMA transfer finishes. It then
*   refreshes the DMA transfer by activating another ping pong DMA cycle.
*
* @notes
*   The userPtr parameter is not used because we don't need a user defined
*   pointer. However, the function definition needs to have all three of
*   these parameters because the default handler provided in em_dma.c calls
*   the user's callback function this way. Alternatively, one could write
*   their own interrupt handler by defining the following macro:
*   EXCLUDE_DEFAULT_DMA_IRQ_HANDLER.
*
* @notes
*   isPrimaryDescriptor gets toggled in the default handler so that the
*   alternate and primary descriptors can get refreshed in sequential order
******************************************************************************/
void refreshTransfer(uint32_t channelNum,
                     bool isPrimaryDescriptor,
                     void *userPtr)
{
  DMA_RefreshPingPong(channelNum,
                      isPrimaryDescriptor,
                      false,                        // Don't use the burst feature
                      (void *) &TIMER0->CC[0].CCVB, // Destination address to transfer to
                      (void *) &buffer,             // Source address to transfer from
                      BUFFER_SIZE-1,                // Number of DMA transfers minus 1
                      false);                       // Don't stop after using this descriptor
}

/**************************************************************************//**
 * @brief
 *    Configure and start DMA
 *****************************************************************************/
void initDma(void)
{
  // Initializing the DMA
  DMA_Init_TypeDef init;
  init.hprot = 0; // Access level/protection not an issue
  init.controlBlock = dmaControlBlock; // Make sure control block is properly aligned
  DMA_Init(&init);

  // Configure DMA callback function to refresh (loop) the transfer
  static DMA_CB_TypeDef callback;
  callback.cbFunc = (DMA_FuncPtr_TypeDef) refreshTransfer;
  callback.userPtr = NULL; // Not needed

  // Channel configuration
  DMA_CfgChannel_TypeDef channelConfig;
  channelConfig.highPri   = false; // Set high priority for the channel
  channelConfig.enableInt = true;  // Interrupt needed because no support for loop mode
  channelConfig.select    = DMAREQ_TIMER0_UFOF; // Select DMA trigger
  channelConfig.cb        = &callback;          // Need callback to refresh the DMA transfer
  uint32_t channelNum     = 0;
  DMA_CfgChannel(channelNum, &channelConfig);

  // Channel descriptor configuration
  static DMA_CfgDescr_TypeDef descriptorConfig;
  descriptorConfig.dstInc  = dmaDataIncNone; // Destination doesn't move
  descriptorConfig.srcInc  = dmaDataInc2;    // Source moves 2 bytes each time
  descriptorConfig.size    = dmaDataSize2;   // Transfer 2 bytes each time
  descriptorConfig.arbRate = dmaArbitrate1;  // Arbitrate after every DMA transfer
  descriptorConfig.hprot   = 0;              // Access level/protection not an issue
  DMA_CfgDescr(channelNum, true, &descriptorConfig);  // Primary descriptor
  DMA_CfgDescr(channelNum, false, &descriptorConfig); // Alternate descriptor

  // Activate ping pong DMA cycle (used for memory-peripheral transfers)
  bool isUseBurst = false;
  DMA_ActivatePingPong(channelNum,
                       isUseBurst,
                       (void *) &TIMER0->CC[0].CCVB, // Primary destination address to transfer to
                       (void *) &buffer,             // Primary source address to transfer from
                       BUFFER_SIZE - 1,              // Primary number of DMA transfers minus 1
                       (void *) &TIMER0->CC[0].CCVB, // Alternate destination address to transfer to
                       (void *) &buffer,             // Alternate source address to transfer from
                       BUFFER_SIZE - 1);             // Alternate number of DMA transfers minus 1
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGpio();
  initTimer();

  // Initialize DMA only after buffer is populated
  populateBuffer();
  initDma();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (will only exit to execute the interrupt
                    // handler that refreshes the DMA transfer)
  }
}

