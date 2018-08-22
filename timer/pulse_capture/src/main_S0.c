/**************************************************************************//**
 * @file
 * @brief This project demonstrates high frequency single pulse capture using
 * the TIMER module. A periodic input signal is routed to a Compare/Capture
 * channel, and a single period is captured and stored. Connect a periodic
 * signal to GPIO pin PD6 (expansion header pin 16) for input. Note minimum
 * duration measurable via this method of input capture is around 700 ns.
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
#include "em_gpio.h"
#include "em_timer.h"
#include "bsp.h"

// Stored edges from interrupt
volatile uint32_t first_edge;
volatile uint32_t second_edge;

/**************************************************************************//**
 * @brief TIMER1_IRQHandler
 * Interrupt Service Routine TIMER1 Interrupt Line
 *****************************************************************************/
void TIMER1_IRQHandler(void)
{
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);

  // Read the last two captured edges
  // Note interrupt occurs after the second capture
  first_edge = TIMER_CaptureGet(TIMER1, 0);
  second_edge = TIMER_CaptureGet(TIMER1, 0);
  
  // Disable subsequent interrupts
  TIMER_IntDisable(TIMER1, TIMER_IF_CC0);
}


/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TIMER1 CC0 Location 4 (PD6) as input
  GPIO_PinModeSet(gpioPortD, 6, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{

  // Configure TIMER1 to run off the HFXO, which runs at 25MHz
  // Enable oscillator and wait for it to stabilize
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  // Set the HFXO as the clock source
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  // Set interrupt for every other edge
  timerCCInit.eventCtrl = timerEventEvery2ndEdge;
  timerCCInit.edge = timerEdgeBoth;
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Route TIMER1 CC0 to location 4 and enable CC0 route pin
  // TIM1_CC0 #4 is GPIO Pin PD6 (Expansion Header Pin 6)
  TIMER1->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC4);;

  // Initialize timer with defined prescale value
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(TIMER1, &timerInit);

  // Safely enable TIMER1 interrupts
  TIMER_IntClear(TIMER1, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(TIMER1_IRQn);
  TIMER_IntEnable(TIMER1, TIMER_IF_CC0);
  NVIC_EnableIRQ(TIMER1_IRQn);
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
