/**************************************************************************//**
 * @file
 * @brief This project demonstrates period measurement using input capture.
 * A periodic input signal is routed to a Compare/Capture channel, and each
 * period length is calculated from the captured edges. Connect a periodic
 * signal to GPIO pin PC10 (expansion header pin 16) for input. Note maximum
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
#define HFPERCLK_IN_MHZ 19
#define WTIMER0_PRESCALE timerPrescale1

// Most recent measured period in microseconds
volatile uint32_t measured_period;
// Stored edge from previous interrupt
volatile uint32_t last_captured_edge;
// Number of wtimer overflows since last interrupt;
volatile uint32_t overflow_count;

/**************************************************************************//**
 * @brief WTIMER0_IRQHandler
 * Interrupt Service Routine WTIMER0 Interrupt Line
 *****************************************************************************/
void WTIMER0_IRQHandler(void)
{
  // Get pending flags and clear
  int irq_flags = TIMER_IntGet(WTIMER0);
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0 | TIMER_IF_OF);

  // Read the last captured value from the CC register
  uint32_t current_edge = TIMER_CaptureGet(WTIMER0, 0);

  // Check if wtimer overflow occurred
  if(irq_flags & TIMER_IF_OF)
  {
    overflow_count++;
  }

  if(irq_flags & TIMER_IF_CC0)
  {
    // Calculate period in microseconds, while compensating for overflows
    // Interrupt latency will affect measurements for periods below 3 microseconds (333 kHz)
    measured_period = (overflow_count*(TIMER_TopGet(WTIMER0) + 2) - last_captured_edge + current_edge)
	/ (HFPERCLK_IN_MHZ * (1 << WTIMER0_PRESCALE));

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

  // Configure WTIMER0 CC0 Location 15 (PC10) as input
  GPIO_PinModeSet(gpioPortC, 10, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief WTIMER initialization
 *****************************************************************************/
void initWTIMER(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  TIMER_InitCC_TypeDef wtimerCCInit = TIMER_INITCC_DEFAULT;
  // Set interrupt on falling edge
  wtimerCCInit.eventCtrl = timerEventEveryEdge;
  wtimerCCInit.edge = timerEdgeFalling;
  wtimerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(WTIMER0, 0, &wtimerCCInit);

  // Route WTIMER0 CC0 to location 15 and enable CC0 route pin
  // WTIM0_CC0 #30 is GPIO Pin PC10 (Expansion Header Pin 16)
  WTIMER0->ROUTELOC0 |= TIMER_ROUTELOC0_CC0LOC_LOC30;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Initialize wtimer with defined prescale value
  TIMER_Init_TypeDef wtimerInit = TIMER_INIT_DEFAULT;
  wtimerInit.prescale = WTIMER0_PRESCALE;
  TIMER_Init(WTIMER0, &wtimerInit);

  // Safely enable WTIMER0 interrupts
  TIMER_IntClear(WTIMER0, TIMER_IF_CC0 | TIMER_IF_OF);
  NVIC_ClearPendingIRQ(WTIMER0_IRQn);
  TIMER_IntEnable(WTIMER0, TIMER_IF_CC0 | TIMER_IF_OF);
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
