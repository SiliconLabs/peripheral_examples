/**************************************************************************//**
 * @file
 * @brief This project demonstrates high frequency single pulse capture using
 * the WTIMER module. A periodic input signal is routed to a Compare/Capture
 * channel, and a single period is captured and stored. Connect a periodic
 * signal to GPIO pin PC10 (expansion header pin 16) for input. Note minimum
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
 * @brief WTIMER0_IRQHandler
 * Interrupt Service Routine WTIMER0 Interrupt Line
 *****************************************************************************/
void WTIMER0_IRQHandler(void)
{
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);

  // Read the last two captured edges
  // Note interrupt occurs after the second capture
  first_edge = TIMER_CaptureGet(WTIMER0, 0);
  second_edge = TIMER_CaptureGet(WTIMER0, 0);
  
  // Disable subsequent interrupts
  TIMER_IntDisable(WTIMER0, TIMER_IF_CC0);
}


/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure WTIMER0 CC0 Location 15 (PC10) as input
  GPIO_PinModeSet(gpioPortC, 10, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief WTIMER initialization
 *****************************************************************************/
void initWTIMER(void)
{

  // Configure WTIMER0 to run off the HFXO, which runs at 40MHz
  // Enable oscillator and wait for it to stabilize
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  // Set the HFXO as the clock source
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  TIMER_InitCC_TypeDef wtimerCCInit = TIMER_INITCC_DEFAULT;
  // Set interrupt for every other edge
  wtimerCCInit.eventCtrl = timerEventEvery2ndEdge;
  wtimerCCInit.edge = timerEdgeBoth;
  wtimerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(WTIMER0, 0, &wtimerCCInit);

  // Route WTIMER0 CC0 to location 15 and enable CC0 route pin
  // WTIM0_CC0 #30 is GPIO Pin PC10 (Expansion Header Pin 16)
  WTIMER0->ROUTELOC0 |= TIMER_ROUTELOC0_CC0LOC_LOC30;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Initialize wtimer with defined prescale value
  TIMER_Init_TypeDef wtimerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(WTIMER0, &wtimerInit);

  // Safely enable WTIMER0 interrupts
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0);
  NVIC_ClearPendingIRQ(WTIMER0_IRQn);
  TIMER_IntEnable(WTIMER0, TIMER_IF_CC0);
  NVIC_EnableIRQ(WTIMER0_IRQn);
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
  initWTIMER();

  while (1)
  {
    EMU_EnterEM1();
  }
}
