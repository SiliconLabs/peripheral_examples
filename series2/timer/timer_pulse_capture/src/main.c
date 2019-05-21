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

// HFXO frequency set for most common radio configurations
#define HFXO_FREQ               38400000

// CTUNE value for the BRD4181A HFXO oscillator
#define HFXO_CTUNE_VALUE        120

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
 *    HFXO initialization
 *****************************************************************************/
void initHFXO(void)
{
  // Initialization structure for HFXO configuration
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Apply board specific tuning capacitor values
  hfxoInit.ctuneXiAna = HFXO_CTUNE_VALUE;
  hfxoInit.ctuneXoAna = HFXO_CTUNE_VALUE;

  // Configure HFXO settings
  CMU_HFXOInit(&hfxoInit);

  // Set system HFXO frequency
  SystemHFXOClockSet(HFXO_FREQ);

  // Enable HFXO oscillator, and wait for it to be stable
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure TIMER0 CC0 Location 15 (PA6) as input
  GPIO_PinModeSet(gpioPortA, 6, gpioModeInput, 0);
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
  // Configure Timer clock source to use the HFXO (EM01GRPA)
  CMU_ClockSelectSet(cmuClock_TIMER0, cmuSelect_HFXO);

  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize the timer
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  // Configure the TIMER0 module for Capture mode and to trigger on every other edge
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  timerInit.enable = false;
  timerInit.clkSel = timerClkSelHFPerClk;
  timerCCInit.eventCtrl = timerEventEvery2ndEdge;
  timerCCInit.edge = timerEdgeBoth; // Trigger an input capture on every edge
  timerCCInit.mode = timerCCModeCapture;

  TIMER_Init(TIMER0, &timerInit);

  // Route Timer0 CC0 output to PA6
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortA << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (6 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Enable TIMER0 interrupts
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
  NVIC_EnableIRQ(TIMER0_IRQn);

  // Enable the TIMER
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Startup and select HFXO as clock source for system and EM01GRPA
  initHFXO();

  // Initializations
  initGpio();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}
