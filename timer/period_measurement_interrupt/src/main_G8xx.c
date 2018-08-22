/**************************************************************************//**
 * @file
 * @brief This project demonstrates period measurement using input capture.
 * A periodic input signal is routed to a Compare/Capture channel, and each
 * period length is calculated from the captured edges. Connect a periodic
 * signal to GPIO pin PD1 (Expansion Header Pin 6) for input. Note maximum
 * measurable frequency is 333 kHz.
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

// Default clock and prescale values
#define HFPERCLK_IN_MHZ 14
#define TIMER0_PRESCALE timerPrescale1

// Most recent measured period in microseconds
volatile uint32_t measured_period;
// Stored edge from previous interrupt
volatile uint32_t last_captured_edge;
// Number of timer overflows since last interrupt;
volatile uint32_t overflow_count;

/**************************************************************************//**
 * @brief TIMER0_IRQHandler
 * Interrupt Service Routine TIMER0 Interrupt Line
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  // Get pending flags and clear
  int irq_flags = TIMER_IntGet(TIMER0);
  TIMER_IntClear(TIMER0, TIMER_IF_CC0 | TIMER_IF_OF);

  // Read the last captured value from the CC register
  uint32_t current_edge = TIMER_CaptureGet(TIMER0, 0);

  // Check if timer overflow occurred between edges
  if(irq_flags & TIMER_IF_OF)
  {
    overflow_count++;
  }

  if(irq_flags & TIMER_IF_CC0)
  {
    // Calculate period in microseconds, while compensating for overflows
    // Interrupt latency will affect measurements for periods below 3 microseconds (333 kHz)
    measured_period = (overflow_count*(TIMER_TopGet(TIMER0) + 2) - last_captured_edge + current_edge)
  / (HFPERCLK_IN_MHZ * (1 << TIMER0_PRESCALE));

    // Store edge for next period
    last_captured_edge = current_edge;
    // Reset overflow count
    overflow_count = 0;
  }
}


/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TIMER0 CC0 location 3 (PD1) as input
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTIMER(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  // Set interrupt on falling edge
  timerCCInit.eventCtrl = timerEventEveryEdge;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Route TIMER0 CC0 to location 3 and enable CC0 route pin
  // TIM0_CC0 #3 is GPIO Pin PD1 (Expansion Header Pin 6)
  TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC3);

  // Initialize timer with defined prescale value
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = TIMER0_PRESCALE;
  TIMER_Init(TIMER0, &timerInit);

  // Safely enable TIMER0 interrupts
  TIMER_IntClear(TIMER0, TIMER_IF_CC0 | TIMER_IF_OF);
  NVIC_ClearPendingIRQ(TIMER0_IRQn);
  TIMER_IntEnable(TIMER0, TIMER_IF_CC0 | TIMER_IF_OF);
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
