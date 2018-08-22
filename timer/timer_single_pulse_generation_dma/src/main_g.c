/**************************************************************************//**
 * @main_g.c
 * @brief The DMA is configured to transfer a single word to the CCV register
 * once the first compare event occurs. The values loaded into the CCV are such
 * that a 1ms pulse is generated after a 1 second delay
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

// Note: change this to change the number of seconds to delay the pulse going high
#define NUM_SEC_DELAY 1

// Note: change this to change the timer prescale
#define TIMER_PRESCALE timerPrescale1024

// Note: change this to change the pulse width (in units of ms)
#define PULSE_WIDTH 1

// Compare values for outputting the rising and falling edge
static uint32_t compareValue1;
static uint32_t compareValue2;

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set PD6 as input
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    Configure and start DMA
 *
 * @note
 *    The channel configuration and descriptor configuration need to have static
 *    scope so that references to them will still be valid after the first DMA
 *    transfer
 *****************************************************************************/
void initDma(void)
{
  // Initializing the DMA
  DMA_Init_TypeDef init;
  init.hprot = 0; // Access level/protection not an issue
  init.controlBlock = dmaControlBlock; // Make sure control block is properly aligned
  DMA_Init(&init);

  // Channel configuration
  static DMA_CfgChannel_TypeDef channelConfig;
  channelConfig.highPri   = false; // Set high priority for the channel
  channelConfig.enableInt = false; // Interrupt not needed in loop transfer mode
  channelConfig.select    = DMAREQ_TIMER0_CC0; // Select DMA trigger
  channelConfig.cb        = NULL;              // No callback because no interrupt
  uint32_t channelNum     = 0;
  DMA_CfgChannel(channelNum, &channelConfig);

  // Channel descriptor configuration
  static DMA_CfgDescr_TypeDef descriptorConfig;
  descriptorConfig.dstInc  = dmaDataIncNone; // Destination doesn't move
  descriptorConfig.srcInc  = dmaDataIncNone; // Source doesn't move
  descriptorConfig.size    = dmaDataSize4;   // Transfer 4 bytes each time
  descriptorConfig.arbRate = dmaArbitrate1;  // Arbitrate after every DMA transfer
  descriptorConfig.hprot   = 0;              // Access level/protection not an issue
  bool isPrimaryDescriptor = true;
  DMA_CfgDescr(channelNum, isPrimaryDescriptor, &descriptorConfig);

  // Initialize and start the DMA transfer
  bool isUseBurst = false;
  DMA_ActivateBasic(channelNum,
                    isPrimaryDescriptor,
                    isUseBurst,
                    (void *) &TIMER0->CC[0].CCV,   // Destination address to transfer to
                    (void *) &compareValue2,       // Source address to transfer from
                    0);                            // Number of DMA transfers minus 1
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
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle; // Toggle output on compare match
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set route to Location 3 and enable
  // TIM0_CC0 #3 is PD1
  TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC3);

  // Initialize and start timer with highest prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.enable = false;
  timerInit.prescale = TIMER_PRESCALE;
  timerInit.oneShot = true; // Generate only one pulse
  TIMER_Init(TIMER0, &timerInit);

  // Set the first compare value
  compareValue1 = CMU_ClockFreqGet(cmuClock_TIMER0)
                    * NUM_SEC_DELAY
                    / (1 << TIMER_PRESCALE);
  TIMER_CompareSet(TIMER0, 0, compareValue1);

  // Set the second compare value (don't actually use it, just set the global so
  // that it can be used by the handler later)
  compareValue2 = (CMU_ClockFreqGet(cmuClock_TIMER0)
                    * PULSE_WIDTH
                    / 1000
                    / (1 << TIMER_PRESCALE))
                    + compareValue1;

  // Enable the TIMER
  TIMER_Enable(TIMER0, true);
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
  initDma();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 mode (won't exit)
  }
}

