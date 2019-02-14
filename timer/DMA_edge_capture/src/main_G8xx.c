/**************************************************************************//**
 * @file
 * @brief This project demonstrates edge capture with DMA. The first 512 events
 * captured by TIMER0 CC0 are transferred to a fixed length buffer by the
 * uDMA. For this example both rising and falling edges are captured.
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
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"
#include "bsp.h"

#define TIMER0_PRESCALE timerPrescale1;
#define BUFFERSIZE 512

// Edge capture buffer
volatile uint32_t buffer[BUFFERSIZE];

/**************************************************************************//**
 * @brief Configure and start DMA
 *****************************************************************************/
void initDMA(void)
{
  // Configure basic DMA settings
  DMA_Init_TypeDef dmaInit;
  dmaInit.hprot = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  // Configure Channel to read from CC0 and disable interrupts
  DMA_CfgChannel_TypeDef  chnlCfg;
  chnlCfg.highPri   = false;
  chnlCfg.enableInt = false;
  chnlCfg.select    = DMAREQ_TIMER0_CC0;
  DMA_CfgChannel(0, &chnlCfg);

  // Configure Channel to read and store CC0 data (16 bits) as a 32 bit number
  DMA_CfgDescr_TypeDef    descrCfg;
  descrCfg.dstInc  = dmaDataInc4;
  descrCfg.srcInc  = dmaDataIncNone;
  descrCfg.size    = dmaDataSize4;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(0, true, &descrCfg);

  // Initialize and Start the DMA transfer
  DMA_ActivateBasic(0, true, false, (void *)&buffer, (void *)&(TIMER0->CC[0].CCV), BUFFERSIZE - 1);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set TIM0_CC0 #3 GPIO Pin (PD1) as Input
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);

  // Configure LED0 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure TIMER0 Compare/Capture settings
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  // Set Interrupt flag on every edge
  timerCCInit.eventCtrl = timerEventEveryEdge;
  timerCCInit.edge = timerEdgeBoth;
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Route TIMER0 CC0 to location 3 and enable CC0 route pin
  // TIM0_CC0 #3 is GPIO Pin PD1
  TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC3);

  // Initialize timer with defined prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = TIMER0_PRESCALE;
  TIMER_Init(TIMER0, &timerInit);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initDMA();
  initGPIO();
  initTIMER();

  while (1)
  {
    EMU_EnterEM1();
  }
}
