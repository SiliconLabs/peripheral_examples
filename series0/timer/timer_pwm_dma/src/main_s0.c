/**************************************************************************//**
 * @main_series0.c
 * @brief This project demonstrates DMA driven pulse width modulation using the
 * TIMER module. The GPIO pin specified in the readme.txt is configured to output
 * a 1kHz signal. The DMA continuously updates the CCVB register to vary the
 * duty cycle.
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
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"

// Note: change this to set the desired Output frequency in Hz
#define PWM_FREQ 1000

// Buffer size
#define BUFFER_SIZE 10

// Note: change this to change the duty cycles used in this example
static const uint16_t dutyCyclePercentages[BUFFER_SIZE] =
  {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

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

  // Configure PD6 as output
  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Configure TIMER1 Compare/Capture for output compare
  // Use PWM mode, which sets output on overflow and clears on compare events
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Route TIMER1 CC0 to location 4 and enable CC0 route pin
  // TIM1_CC0 #4 is GPIO Pin PD6
  TIMER1->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC4);

  // Set top value to overflow at the desired PWM_FREQ frequency
  TIMER_TopSet(TIMER1, CMU_ClockFreqGet(cmuClock_TIMER1) / PWM_FREQ);

  // Initialize and start timer with no prescaling
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(TIMER1, &timerInit);

  // Trigger DMA on compare event to set CCVB to update duty cycle on next period
  TIMER_IntEnable(TIMER1, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief
 *    Populate buffer with timer duty cycle values
 *****************************************************************************/
void populateBuffer(void)
{
  for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = (uint16_t) (TIMER_TopGet(TIMER1) * dutyCyclePercentages[i] / 100);
  }
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

  // Channel configuration
  DMA_CfgChannel_TypeDef channelConfig;
  channelConfig.highPri   = false; // Set high priority for the channel
  channelConfig.enableInt = false; // Interrupt not needed in loop transfer mode
  channelConfig.select    = DMAREQ_TIMER1_CC0; // Select DMA trigger
  channelConfig.cb        = NULL;              // No callback because no interrupt
  uint32_t channelNum     = 0;
  DMA_CfgChannel(channelNum, &channelConfig);

  // Channel descriptor configuration
  static DMA_CfgDescr_TypeDef descriptorConfig;
  descriptorConfig.dstInc  = dmaDataIncNone; // Destination doesn't move
  descriptorConfig.srcInc  = dmaDataInc2;    // Source moves 2 bytes each time
  descriptorConfig.size    = dmaDataSize2;   // Transfer 2 bytes each time
  descriptorConfig.arbRate = dmaArbitrate1;  // Arbitrate after every DMA transfer
  descriptorConfig.hprot   = 0;              // Access level/protection not an issue
  bool isPrimaryDescriptor = true;
  DMA_CfgDescr(channelNum, isPrimaryDescriptor, &descriptorConfig);

  // Configure loop transfer mode
  DMA_CfgLoop_TypeDef loopConfig;
  loopConfig.enable  = true;            // Enable looping
  loopConfig.nMinus1 = BUFFER_SIZE - 1; // Reload value for number of DMA transfers minus 1
  DMA_CfgLoop(channelNum, &loopConfig);

  // Activate basic DMA cycle (used for memory-peripheral transfers)
  bool isUseBurst = false;
  DMA_ActivateBasic(channelNum,
                    isPrimaryDescriptor,
                    isUseBurst,
                    (void *) &TIMER1->CC[0].CCVB, // Destination address to transfer to
                    (void *) &buffer,             // Source address to transfer from
                    BUFFER_SIZE - 1);             // Number of DMA transfers minus 1
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
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

