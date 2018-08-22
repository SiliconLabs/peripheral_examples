/**************************************************************************//**
 * @file
 * @brief This project demonstrates polled period measurement using input
 * capture. A periodic input signal is routed to a Compare/Capture channel,
 * and each period length is calculated from the captured edges. Connect a
 * periodic signal to GPIO pin PD6 (expansion header pin 16) for input. Note
 * maximum measurable frequency is 333 kHz.
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
#define TIMER1_PRESCALE timerPrescale1

// Most recent measured period in microseconds
volatile uint32_t measured_period;
// Stored edge from previous edge capture
volatile uint32_t last_captured_edge;
// Number of timer overflows since last edge capture;
volatile uint32_t overflow_count;

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
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  // Input capture on falling edge
  timerCCInit.eventCtrl = timerEventEveryEdge;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Route TIMER1 CC0 to location 4 and enable CC0 route pin
  // TIM1_CC0 #4 is GPIO Pin PD6 (Expansion Header Pin 16)
  TIMER1->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC4);;

  // Initialize timer with defined prescale value
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = TIMER1_PRESCALE;
  TIMER_Init(TIMER1, &timerInit);

  // Enable TIMER1 interrupt flags
  TIMER_IntClear(TIMER1, TIMER_IF_CC0 | TIMER_IF_OF);
  TIMER_IntEnable(TIMER1, TIMER_IF_CC0 | TIMER_IF_OF);
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

  int irq_flags;
  while (1)
  {
    // Wait for flags
    while(!(TIMER_IntGetEnabled(TIMER1) & (TIMER_IF_OF | TIMER_IF_CC0)));

    // Get flags and clear
    irq_flags = TIMER_IntGet(TIMER1);
    TIMER_IntClear(TIMER1, TIMER_IF_CC0 | TIMER_IF_OF);

    // Read the last captured value from the CC register
    uint32_t current_edge = TIMER_CaptureGet(TIMER1, 0);

    // Check if timer overflow occurred between edges
    if(irq_flags & TIMER_IF_OF)
    {
      overflow_count++;
    }

    if(irq_flags & TIMER_IF_CC0)
    {
      // Calculate period in microseconds, while compensating for overflows
      // Loop execution time will affect measurements for periods below 3 microseconds (333 kHz)
      measured_period = (overflow_count*(TIMER_TopGet(TIMER1) + 2) - last_captured_edge + current_edge)
    / (HFPERCLK_IN_MHZ * (1 << TIMER1_PRESCALE));

      // Store edge for next period
      last_captured_edge = current_edge;
      // Reset overflow count
      overflow_count = 0;
    }
  }
}
