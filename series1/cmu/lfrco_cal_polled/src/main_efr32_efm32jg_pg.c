/***************************************************************************//**
 * @file main_efr32_efm32jg_pg.c
 * @brief Demonstrates calibration of the LFRCO against the HFXO with output
 * to a pin.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/
 
#include "em_chip.h"
#include "em_cmu.h"
#include "em_cryotimer.h"
#include "em_emu.h"
#include "em_gpio.h"

/*
 * Top value for the calibration down counter.  Maximum allowed is
 * 2^20-1 = 0xFFFFF, which equates to 2^20 counts.  A larger value
 * translates into greater accuracy at the expense of a longer
 * calibration period
 *
 * In this example, calibration is run for DOWN counts of the HFXO.
 * At the same time, the LFRCO clocks the up counter.  DOWN counts
 * of the HFXO take a certain amount of time at a given frequency:
 *
 *      DOWN
 * -------------- = calibration time
 * HFXO frequency
 *
 * It follows that the up counter will run for some number of counts
 * of the LFRCO during the same period of time:
 *
 *       UP
 * --------------- = calibration time
 * LFRCO frequency
 *
 * So, to calibrate for a specified HFXO frequency, we should expect
 * that...
 *
 *      LFRCO frequency * DOWN
 * UP = ----------------------
 *          HFXO frequency
 *
 * For example, if the desired LFRCO frequency is 32768 Hz, then the up
 * counter should reach...
 *
 * 32768 x 1048576
 * --------------- = 895 (rounded up from 894.8)
 *    38400000
 *
 * ...when clocked from a 38.4 MHz crystal connected to the HFXO.
 *
 * This can be run for any desired frequency to the extent that it is
 * within the range of the LFRCO.
 */
#define DOWNCOUNT   0xFFFFF

/*
 * Setup the CRYOTIMER to generate interrupts at 16-second intervals.
 * The LFRCO is used as the CRYOTIMER clock, but the fact that the
 * clock frequency might change due to calibration isn't really
 * relevant for this example.
 */
void startCryo(void)
{
  CRYOTIMER_Init_TypeDef cryoInit = CRYOTIMER_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_CRYOTIMER, true);

  // Set CRYOTIMER parameters
  cryoInit.osc = cryotimerOscLFRCO;
  cryoInit.presc = cryotimerPresc_128;
  cryoInit.period = cryotimerPeriod_4k;
  cryoInit.enable = true;

  // Interrupt setup
  CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);
  CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
  NVIC_ClearPendingIRQ(CRYOTIMER_IRQn);
  NVIC_EnableIRQ(CRYOTIMER_IRQn);

  CRYOTIMER_Init(&cryoInit);
}

void CRYOTIMER_IRQHandler(void)
{
  // Clear the CRYOTIMER interrupt
  CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);

  /*
   * Force the write to CRYOTIMER_IFC to complete before proceeding to
   * make sure the interrupt is not re-triggered when upon exiting this
   * IRQ handler as the CRYOTIMER is in an asynchronous clock domain.
   */
  __DSB();
}

/*
 * Before calling this function make sure that HFXO and LFRCO are
 * already running.
 *
 * Calibration is an iterative process because the CMU hardware simply
 * returns a count.  It doesn't determine whether or not a specific
 * tuningVal is correct.  To keep things simple, this function implements
 * the search algorithm and not any of the setup that might differ from
 * system to system.
 */
void calLFRCO(uint32_t freq)
{
  bool tuned, lastUpGT, lastUpLT;

  uint32_t idealCount, upCount, prevUp, tuningVal, prevTuning;

  // Get current tuningVal value
  tuningVal = CMU_OscillatorTuningGet(cmuOsc_LFRCO);

  /*
   * Determine the ideal up counter value based on the desired
   * frequency.  Note that this is done to maintain accuracy throughout
   * the calculation.
   *
   * In most applications, it is probably better to calculate the ideal
   * up counter value(s) in advance.  For example, the ideal counter
   * value at the default LFRCO frequency is (when calibrating against
   * the HFXO and using the maximum down counter value of 0xFFFFF):
   *
   * 32768 x 1048576
   * --------------- = 895 (rounded up from 894.8)
   *    38400000
   */
  idealCount = (uint32_t)(((float)SystemLFRCOClockGet() / (float)SystemHFXOClockGet()) * (float)(DOWNCOUNT + 1));

  /*
   * Haven't run a calibration cycle yet so not tuned, and the last up
   * count greater than and less than flags are ALSO false.
   */
  tuned = false;
  lastUpGT = false;
  lastUpLT = false;

  while (!tuned)
  {
    // Setup the calibration circuit
    CMU_CalibrateConfig(DOWNCOUNT, cmuOsc_HFXO, cmuOsc_LFRCO);

    // Start the up counter
    CMU_CalibrateStart();

    // Wait for down counter to finish
    while ((CMU->STATUS & CMU_STATUS_CALRDY) == 0);

    // Get the up counter value
    upCount = CMU_CalibrateCountGet();

    /*
     * If the up counter result is less than the tuned value, the LFRCO
     * is running at a higher frequency, so the tuning value has to be
     * incremented.
     */
    if (upCount < idealCount)
    {
      // Was the up counter greater than the tuned value on the last run?
      if (lastUpGT)
      {
        /*
         * If the difference between the ideal count and the up count
         * from this run is greater than it was on the last run, then
         * the last run produced a more accurate tuning, so revert to
         * the previous tuning value.  If not, the current value gets
         * the most accurate tuning.
         */
        if ((idealCount - upCount) > (prevUp - idealCount))
          tuningVal = prevTuning;

        // done tuning
        tuned = true;
      }
      // Up counter for the last run not greater than the tuned value
      else
      {
        /*
         * If the difference is 1, decrementing the tuning value again
         * will only increase the frequency further away from the
         * intended target, so tuning is now complete.
         */
        if ((idealCount - upCount) == 1)
          tuned = true;
        /*
         * The difference between the ideal count for the desired
         * frequency and the result from this run is > 1, so decrease
         * the tuning value to increase the LFRCO frequency.  Next
         * time, the up counter value will increase.  Save the tuning
         * value from this run; if close, it might be more accurate
         * than the result of the next run.
         */
        else
        {
          prevTuning = tuningVal;
          tuningVal--;
          lastUpLT = true;  // Remember that the up counter was less than the ideal this run
          prevUp = upCount;
        }
      }
    }

    /*
     * If the up counter result is greater than the tuned value, the
     * LFRCO is running at a lower frequency, so the tuning value has
     * to be decremented.
     */
    if (upCount > idealCount)
    {
      // Was the up counter less than the tuned value on the last run?
      if (lastUpLT)
      {
        /*
         * If the difference between the up count and the ideal count
         * from this run is greater than it was on the last run, then
         * the last run produced a more accurate tuning, so revert to
         * the previous tuning value.  If not, the current value gets
         * the most accurate tuning.
         */
        if ((upCount - idealCount) > (idealCount - prevUp))
          tuningVal = prevTuning;

        // Done tuning
        tuned = true;
      }
      // Up counter for the last run not less than the tuned value
      else
      {
        /*
         * If the difference is 1, incrementing the tuning value again
         * will only decrease the frequency further away from the
         * intended target, so tuning is now complete.
         */
        if ((upCount - idealCount) == 1)
          tuned = true;
        /*
         * The difference between the ideal count for the desired
         * frequency and the result from this run is > 1, so increase
         * the tuning value to decrease the LFRCO frequency.  Next
         * time, the up counter value will decrease.  Save the tuning
         * value from this run; if close, it might be more accurate
         * than the result of the next run.
         */
        else
        {
          prevTuning = tuningVal;
          tuningVal++;
          lastUpGT = true;  // Remember that the up counter was less than the ideal this run
          prevUp = upCount;
        }
      }
    }

    // Up counter result is equal to the desired value, end of calibration
    if (upCount == idealCount)
      tuned = true;
    // Otherwise set new tuning value
    else
      CMU_OscillatorTuningSet(cmuOsc_LFRCO, tuningVal);
  }
}

int main(void)
{
  CHIP_Init();

  // Initialize DCDC with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Start the HFXO with safe default parameters
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  CMU_HFXOInit(&hfxoInit);
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  // Switch the HFCLK to the HFXO.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Enable the LFRCO
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

  // Drive LFRCO onto PC11 to observe calibration
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(gpioPortC, 11, gpioModePushPull, 0);
  CMU->ROUTELOC0 = CMU_ROUTELOC0_CLKOUT0LOC_LOC3;
  CMU->CTRL |= CMU_CTRL_CLKOUTSEL0_LFRCOQ;
  CMU->ROUTEPEN = CMU_ROUTEPEN_CLKOUT0PEN;

  // Start the 16-second CRYOTIMER interrupts
  startCryo();

  while(1)
  {
    // Wait for the CRYOTIMER interrupt
    EMU_EnterEM2(true);

    // Run calibration
    calLFRCO(32768);
  }
}
