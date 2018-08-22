/**************************************************************************//**
 * @main_gg11_tg11.c
 * @brief This project demonstrates polled period measurement using input
 * capture. A periodic input signal is routed to a Compare/Capture channel,
 * and each period length is calculated from the captured edges. Connect a
 * periodic signal to the GPIO pin specified in the readme.txt for input.
 * Note maximum measurable frequency is 333 kHz.
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

// Default prescale value
#define TIMER1_PRESCALE timerPrescale1

// Default clock value
#define HFPERCLK_IN_MHZ 19

// Most recent measured period in microseconds
static volatile uint32_t measuredPeriod;

// Stored edge from previous interrupt
static volatile uint32_t lastCapturedEdge;

// Number of timer overflows since last interrupt;
static volatile uint32_t overflowCount;

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TIMER1 CC0 Location 0 (PC13) as input
  GPIO_PinModeSet(gpioPortC, 13, gpioModeInput, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Configure the TIMER1 module for Capture mode and to trigger on falling edges
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Route TIMER1 CC0 to location 0 and enable CC0 route pin
  // TIM1_CC0 #0 is GPIO Pin PC13
  TIMER1->ROUTELOC0 |= TIMER_ROUTELOC0_CC0LOC_LOC0;
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Initialize timer with defined prescale value
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = TIMER1_PRESCALE;
  TIMER_Init(TIMER1, &timerInit);
}

/**************************************************************************//**
 * @brief
 *    Calculate the period of the input waveform using Capture mode on TIMER1
 *    channel 0
 *
 * @return
 *    The period of the input waveform
 *****************************************************************************/
uint32_t calculatePeriod(void)
{
  // Read the capture value from the CC register
  uint32_t current_edge = TIMER_CaptureGet(TIMER1, 0);

  // Check if the timer overflowed (flag gets set even if interrupt not enabled)
  if (TIMER_IntGet(TIMER1) & TIMER_IF_OF) {
    overflowCount++;
    TIMER_IntClear(TIMER1, TIMER_IF_OF);
  }

  // Calculate period in microseconds, while compensating for overflows
  // Interrupt latency will affect measurements for periods below 3 microseconds (333 kHz)
  uint32_t period = (overflowCount * (TIMER_TopGet(TIMER1) + 2)
                     - lastCapturedEdge + current_edge)
                     / (HFPERCLK_IN_MHZ * (1 << TIMER1_PRESCALE));

  // Record the capture value for the next period measurement calculation
  lastCapturedEdge = current_edge;

  // Reset the overflow count
  overflowCount = 0;

  return period;
}

/**************************************************************************//**
 * @brief
 *    Main function
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

    // Wait for a capture event to happen
    while ((TIMER1->STATUS & TIMER_STATUS_ICV0) == 0) {}

    // Record the period into the global variable
    measuredPeriod = calculatePeriod();
  }
}


