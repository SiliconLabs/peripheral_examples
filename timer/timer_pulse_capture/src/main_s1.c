/**************************************************************************//**
 * @main_series1.c
 * @brief This project demonstrates high frequency single pulse capture using
 * the TIMER module. A periodic input signal is routed to a Compare/Capture
 * channel, and a single pulse width is captured and stored. Connect a periodic
 * signal to the GPIO pin specified in the readme.txt for input. Note: minimum
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
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"

// Stored edges from interrupt
volatile uint32_t firstEdge;
volatile uint32_t secondEdge;

/**************************************************************************//**
 * @brief
 *    Interrupt handler for TIMER0
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(TIMER0);
  TIMER_IntClear(TIMER0, flags);

  // Read the last two captured edges
  // Note: interrupt occurs after the second capture
  firstEdge = TIMER_CaptureGet(TIMER0, 0);
  secondEdge = TIMER_CaptureGet(TIMER0, 0);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TIMER0 CC0 Location 15 (PC10) as input
  GPIO_PinModeSet(gpioPortC, 10, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *
 * @details
 *    Configure TIMER0 to run off the HFXO
 *****************************************************************************/
void initTimer(void)
{
  // Enable oscillator and wait for it to stabilize
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  // Set the HFXO as the clock source
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure the TIMER0 module for Capture mode and to trigger on every other edge
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.eventCtrl = timerEventEvery2ndEdge; // Trigger an event (PRS pulse, interrupt flag, or DMA request) every second capture
  timerCCInit.edge = timerEdgeBoth; // Trigger an input capture on every edge
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Route TIMER0 CC0 to location 15 and enable CC0 route pin
  // TIM0_CC0 #15 is GPIO Pin PC10
  TIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC15;
  TIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Initialize timer
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(TIMER0, &timerInit);

  // Enable TIMER0 interrupts
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
  NVIC_EnableIRQ(TIMER0_IRQn);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initializations
  initGpio();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

