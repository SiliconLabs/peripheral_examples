/**************************************************************************//**
 * @file
 * @brief This project demonstrates high frequency single pulse capture using
 * the TIMER module. A periodic input signal is routed to a Compare/Capture
 * channel, and a single period is captured and stored. Connect a periodic
 * signal to GPIO pin PC10 (expansion header pin 16) for input. Note minimum
 * duration measurable via this method of input capture is around 700 ns.
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
#include "bsp.h"

// Stored edges from interrupt
volatile uint32_t first_edge;
volatile uint32_t second_edge;

/**************************************************************************//**
 * @brief TIMER0_IRQHandler
 * Interrupt Service Routine TIMER0 Interrupt Line
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);

  // Read the last two captured edges
  // Note interrupt occurs after the second capture
  first_edge = TIMER_CaptureGet(TIMER0, 0);
  second_edge = TIMER_CaptureGet(TIMER0, 0);
  
  // Disable subsequent interrupts
  TIMER_IntDisable(TIMER0, TIMER_IF_CC0);
}


/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TIMER0 CC0 Location 15 (PC10) as input
  GPIO_PinModeSet(gpioPortC, 10, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{

  // Configure TIMER0 to run off the HFXO, which runs at 40MHz
  // Enable oscillator and wait for it to stabilize
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  // Set the HFXO as the clock source
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  // Set interrupt for every other edge
  timerCCInit.eventCtrl = timerEventEvery2ndEdge;
  timerCCInit.edge = timerEdgeBoth;
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Route TIMER0 CC0 to location 15 and enable CC0 route pin
  // TIM0_CC0 #15 is GPIO Pin PC10 (Expansion Header Pin 16)
  TIMER0->ROUTELOC0 |= TIMER_ROUTELOC0_CC0LOC_LOC15;
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Initialize timer with defined prescale value
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(TIMER0, &timerInit);

  // Safely enable TIMER0 interrupts
  TIMER_IntClear(TIMER0, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(TIMER0_IRQn);
  TIMER_IntEnable(TIMER0, TIMER_IF_CC0);
  NVIC_EnableIRQ(TIMER0_IRQn);
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

  while (1)
  {
    EMU_EnterEM1();
  }
}
