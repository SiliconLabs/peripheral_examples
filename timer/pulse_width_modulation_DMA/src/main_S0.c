/**************************************************************************//**
 * @file
 * @brief This project demonstrates DMA driven pulse width modulation using the
 * TIMER module. PD6 (See readme for Exp. pin out) is configured for output and
 * outputs a 1kHz signal. The LDMA continuously updates the CCVB register to
 * vary the duty cycle.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"
#include "bsp.h"

// Output frequency in Hz
#define PWM_FREQ 1000

#define BUFFERSIZE 10

// Globally declared DMA callback structure
DMA_CB_TypeDef cb;

// Buffer of duty cycle values for DMA transfer to CCVB
// Buffer is populated after TIMER is initialized and Top value is set
uint16_t buffer[BUFFERSIZE];

/**************************************************************************//**
 * @brief  Call-back called when DMA transfer is complete
 *****************************************************************************/
void transferComplete(unsigned int channel, bool primary, void *user)
{
  // Re-activate DMA for next transfer
  DMA_RefreshPingPong(channel,
                      primary,
                      false,
                      NULL,
                      NULL,
                      BUFFERSIZE - 1,
                      false);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD6 (See readme for Exp. pin out) as output
  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Configure TIMER1 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  // use PWM mode, which sets output on overflow and clears on compare events
  timerCCInit.mode = timerCCModePWM;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Route TIMER1 CC0 to location 4 and enable CC0 route pin
  // TIM1_CC0 #4 is GPIO Pin PD6 (Expansion Header Pin 16)
  TIMER1->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC4);

  // Set top value to overflow once per signal period
  TIMER_TopSet(TIMER1, CMU_ClockFreqGet(cmuClock_TIMER1) / PWM_FREQ);

  // Initialize and start timer with no prescaling
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1;
  TIMER_Init(TIMER1, &timerInit);

  // Safely enable TIMER1 CC0 interrupt flags
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(TIMER1_IRQn);

  // Trigger DMA on compare event to set CCVB to update duty cycle on next period
  TIMER_IntEnable(TIMER1, TIMER_IF_CC0);
}

/**************************************************************************//**
 * @brief populate buffer of duty cycle values
 *****************************************************************************/
void populateBuffer(void)
{
  // 100% duty cycle equals the set TIMER1 top value
  int max = TIMER_TopGet(TIMER1);

  for (int i = 0; i < BUFFERSIZE; i++)
  {
    // Buffer increments from 0 to 100% duty cycle
    buffer[i] =  (uint16_t) ((i * max) / (BUFFERSIZE - 1));
  }
}

/**************************************************************************//**
 * @brief Configure and start DMA
 *****************************************************************************/
void initDMA(void)
{
  CMU_ClockEnable(cmuClock_DMA, true);

  // Configure basic DMA settings
  DMA_Init_TypeDef dmaInit;
  dmaInit.hprot = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  // Configure DMA call-back function to loop transfer
  cb.cbFunc  = transferComplete;
  cb.userPtr = NULL;

  // Configure Channel to read from memory and write to CC0 on each compare event
  DMA_CfgChannel_TypeDef  chnlCfg;
  chnlCfg.enableInt = true;
  chnlCfg.cb = &cb;
  chnlCfg.select    = DMAREQ_TIMER1_CC0;
  DMA_CfgChannel(0, &chnlCfg);

  // Configure Channel to store CC0 data (16 bits) as a 32 bit number
  DMA_CfgDescr_TypeDef    descrCfg;
  descrCfg.dstInc  = dmaDataIncNone;
  descrCfg.srcInc  = dmaDataInc2;
  descrCfg.size    = dmaDataSize2;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(0, true, &descrCfg);
  DMA_CfgDescr(0, false, &descrCfg);


  // Initialize and Start the DMA transfer
  DMA_ActivatePingPong(0, false, (void *)&(TIMER1->CC[0].CCVB), (void *)&buffer,
     BUFFERSIZE - 1, (void *)&(TIMER1->CC[0].CCVB), (void *)&buffer, BUFFERSIZE - 1);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGPIO();
  initTIMER();

  // Initialize DMA only after buffer is populated
  populateBuffer();
  initDMA();

  while (1)
  {
    EMU_EnterEM1();
  }
}
