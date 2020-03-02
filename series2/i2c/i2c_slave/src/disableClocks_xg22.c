#include "em_device.h"
#include "em_cmu.h"

/***************************************************************************//**
 * @brief   Disable high frequency clocks
 ******************************************************************************/
static void disableHFClocks(void)
{
  // Disable high frequency peripherals
  CMU_ClockEnable(cmuClock_TIMER0, false);
  CMU_ClockEnable(cmuClock_TIMER1, false);
  CMU_ClockEnable(cmuClock_TIMER2, false);
  CMU_ClockEnable(cmuClock_TIMER3, false);
  CMU_ClockEnable(cmuClock_TIMER4, false);
  CMU_ClockEnable(cmuClock_PDM, false);
  CMU_ClockEnable(cmuClock_EUART0, false);
  CMU_ClockEnable(cmuClock_IADC0, false);
}

/***************************************************************************//**
 * @brief   Disable low frequency clocks
 ******************************************************************************/
static void disableLFClocks(void)
{
  // Disable low frequency peripherals
  CMU_ClockEnable(cmuClock_LETIMER0, false);
  CMU_ClockEnable(cmuClock_WDOG0, false);
  CMU_ClockEnable(cmuClock_RTCC, false);
  CMU_ClockEnable(cmuClock_BURTC, false);
}

/***************************************************************************//**
 * @brief   Disable all clocks to achieve lowest current consumption numbers.
 ******************************************************************************/
extern void disableClocks(void)
{
  // Disable High Frequency Clocks
  disableHFClocks();

  // Disable Low Frequency Clocks
  disableLFClocks();
}
