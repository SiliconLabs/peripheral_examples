/***************************************************************************//**
 * @file gg1x_main_rtcc_em4_wake.c
 * @brief This project demonstrates use of the RTCC to wake from EM4.
 *
 * Pay attention to the steps performed during initialization and how
 * they differ for wake from EM4 vs. other reset sources.
 *
 * Once the device is initialized, it toggles LED0 on or off depending
 * on its last state, enters EM4 for 5 seconds, and then wakesup.
 *
 * Because it is not possible to connect to a device in EM4, there is
 * also an "escape hatch" mechanism available  Hold down PB0, and upon
 * wake from EM4, the device will turn on LED0 and LED1 and execute
 * the BKPT instruction.  This stops the processor while it is still
 * in EM0, thus allowing a debugger to make a connection.
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_rmu.h"
#include "em_rtcc.h"

#include "bsp.h"

/*
 *  Specify the LFXO frequency in Hz.  This provides flexibility for
 *  configuring the RTCC so as to accommodate non-standard frequencies
 *  like 32 kHz at the expensive of losing the use of capture/compare
 *  channel 0.
 */
#define LFXOFREQ	32768

/**************************************************************************//**
 * @brief
 *    Initialize the LF domain clocks.
 *****************************************************************************/
void lfClkInit(bool unlatch)
{
  /*
   * Clocks in the low-energy domain have to enabled in a specific
   * order, especially after waking from EM4.  Before doing anything
   * else, the bus clock for the low-energy domain registers needs
   * to be enabled.
   */
  CMU_ClockEnable(cmuClock_HFLE, true);

  /*
   * Again, wake from EM4 requires special handling.  It is customary
   * to use CMU_ClockSelectSet() to select the oscillator for one of
   * the LFxCLK domains, but this function also enables the selected
   * oscillator.  This CANNOT be done after waking from EM4 because
   * certain registers are latched and must be unlatched.
   *
   * Failure to do will result in code getting stuck when
   * CMU_ClockSelectSet() calls CMU_OscillatorEnable() to enable the
   * LFXO.  Until the unlatch operation is performed,
   * CMU_OscillatorEnable() will sit in a loop waiting for
   * CMU_STATUS_LFXOENS to go active, which will never happen.
   *
   * So, to properly manage this, we want to manually select the LFXO
   * as the source of the LFECLK, but we'll hold off on enabling the
   * LFXO until we've done this.
   *
   * Note that any LF oscillator retained in EM4 remains running upon
   * wake, but the registers that control that oscillator and report
   * its status don't function normally until unlatch is performed.
   */
  CMU->LFECLKSEL = CMU_LFECLKSEL_LFE_LFXO;

  // (Re)initialize the LFXO before enabling it.
  CMU_LFXOInit_TypeDef lfxo = CMU_LFXOINIT_DEFAULT;
  CMU_LFXOInit(&lfxo);

  /*
   * Every register that needs to be re-initialized before unlatch
   * should now be handled, so unlatch can now be performed without
   * affecting anything that that was retained through wake.
   */
  if (unlatch)
    EMU_UnlatchPinRetention();

  // It is now safe to restart the LFXO
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
}

/**************************************************************************//**
 * @brief
 *    Initialize the RTCC.
 *****************************************************************************/
void rtccInit(uint32_t next)
{
  // (Re-)enable the RTCC bus clock
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Configure the RTCC but don't enable yet
  RTCC_Init_TypeDef rtcc = RTCC_INIT_DEFAULT;
  rtcc.enable   		    = false;
  rtcc.debugRun			    = false;
  rtcc.precntWrapOnCCV0 = true;
  rtcc.cntWrapOnCCV1 	  = false;
  rtcc.presc            = rtccCntPresc_1;
  rtcc.prescMode        = rtccCntTickCCV0Match;
  rtcc.cntMode          = rtccCntModeNormal;
  rtcc.enaOSCFailDetect	= false;

  /*
   * Set RTC_CC0_CCV to frequency of the LFXO crystal used (minus 1).
   * When the pre-counter (RTCC_PRECNT) matches this value, the counter
   * (RTCC_CNT) increments by one.
   *
   * Take special note of the fact that when using the PRECENT match
   * feature, the actual comparison is to RTC_CC0_CCV bits [14:0]
   * because the pre-counter is 15 bits wide while the capture/compare
   * value register is 32 bits wide.
   *
   * This also why LFXOFREQ - 1 is written to CC0_CCV.  In the case of
   * a 32.768 kHz crystal, writing 32768 (0x8000) to CC0_CCV never
   * results in a match because bit 15 is not compared, thus CNT
   * increments at the frequency of the LFECLK.
   *
   * We're using this option instead of a 2^n prescaler tap selectable
   * by RTCC_CTRL_CNTPRESC in order to accommodate non-standard LFXO
   * crystal frequencies, like 32 kHz.
   */
  RTCC->CC[0].CCV = LFXOFREQ - 1;

  // Initialize and start counting
  RTCC_Init(&rtcc);

  /*
   * Set up RTCC capture/compare channel 1 to match at the specified
   * number of ticks after the current count, which will be 'next'
   * seconds later because CNT increments once per LFXOFREQ - 1 counts
   * of PRECENT.
   */
  RTCC_CCChConf_TypeDef compCfg = RTCC_CH_INIT_COMPARE_DEFAULT;
  RTCC_ChannelInit(1, &compCfg);
  RTCC_ChannelCompareValueSet(1, RTCC_CounterGet() + next);

  /*
   * Enable capture/compare channel 1 match interrupt.  Note that this
   * example spends most of its time in EM4, so we're not actually
   * enabling the NVIC RTCC interrupt source.  No processor interrupt
   * is ever going to be generated.  We're enabling this interrupt to
   * use it to wake the device from EM4.
   */
  RTCC_IntClear(_RTCC_IFC_MASK);
  RTCC_IntEnable(RTCC_IEN_CC1);
  RTCC_EM4WakeupEnable(true);

  // Now start counting
  RTCC_Enable(true);

  /*
   * The following lines of code are a crutch of sorts needed solely
   * for this example.  The RTCC is enabled by setting the ENABLE bit
   * in the RTCC_CTRL register.  A write to this bit must cross the
   * clock domain boundary between the peripheral registers, which are
   * clocked by a local copy of the HFBUSCLKLE, and the RTCC logic,
   * which is clocked by the LFECLK.
   *
   * While a write to RTCC_CTRL_ENABLE does not stall the CPU (e.g.
   * the RTCC does not delay completion of the transfer pending
   * acknowledgement of the write in the LFECLK domain), it does
   * not take effect immediately.  Consequently, if the code enters EM4
   * immediately after the write to ENABLE, the chip will never wake
   * because the counter will not have started.
   *
   * For code where this might be a concern, there are a few different
   * ways to handle it.  One option would be to simply wait for the
   * first tick to register in the CNT register.  In this example,
   * we clear the STATUS register BUMODETS bit, which requires a write
   * to the RTC_CMD_CLEARSTATUS bit.  The CMD bit in the SYNCBUSY
   * registers the completion of this write and can be used to confirm
   * that the RTCC is enabled.
   */
  RTCC->CMD = RTCC_CMD_CLRSTATUS;
  while ((RTCC->SYNCBUSY & RTCC_SYNCBUSY_CMD));
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  uint32_t rflags, pinstate;
  bool wakestate;

  CHIP_Init();

  /*
   * Regardless of whether we are booting from POR or waking from EM4,
   * turn on the GPIO clock here.  It's going to be needed no matter
   * what else has to be done during initialization and saves us from
   * having to write redundant code to do this.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Get reset cause(s)
  rflags = RMU_ResetCauseGet();

  /*
   * What caused the last reset?
   *
   * Keep in mind that for any given reset there can be multiple causes.
   *
   * Any source that causes a wake from EM4 (e.g. a GPIO pin or the
   * CRYOTIMER) sets the RMU_RSTCAUSE register's EM4RST bit.  For this
   * reason, assertion of the nRESET pin while in EM4 sets both the
   * EXTRST and EM4RST bits in RMU_RSTCAUSE.
   *
   * In this demonstration, we want to know if the last reset was caused
   * ONLY by RTCC wake from EM4 and not something like the nRESET pin,
   * so the code tests exclusively for EM4RST and not other
   * RMU_RSTCAUSE flags that could also be set at the same time.
   */
  if (rflags == RMU_RSTCAUSE_EM4RST)
  {
    /*
     * If we're here, we woke from EM4, which we need to take note of
     * in order to determine whether or not to perform an unlatch
     * operation when initializing the LF clock domains and GPIO.
     */
    wakestate = true;

    /*
     * It may be necessary to retain the state of the system in order
     * reconfigure peripherals or otherwise change the behavior of the
     * software.
     *
     * One way to do this is to simply store anything that needs to be
     * saved in the retention RAM, but this comes at the cost of a few
     * tenths of a microamp of added current draw (EM4H current without
     * the RTCC vs. EM4S current).
     *
     * For systems with much simpler requirements, state can be
     * retained by using the RESETSTATE bit field in the RMU_CTRL
     * register.  RESETSTATE is a 2-bit field that is retained through
     * all resets except POR and hard pin reset.
     *
     * This example uses RESETSTATE to specify whether the LED that
     * toggles upon each wake needs to be on or off.
     */
    pinstate = RMU_UserResetStateGet();

    // Toggle the LED state and save it for the next wake
    if (pinstate == 0)
      pinstate = 1;
    else
      pinstate = 0;

    RMU_UserResetStateSet(pinstate);
  }
  /*
   * If we get here, it means we had a reset source other than (although
   * in addition to is possible) EM4RST.  This could be something more
   * severe, like POR, which would mean that if the RTCC were being used
   * to keep time, it would need to be reset, something that wouldn't be
   * required in the case of a proper EM4 wake.
   */
  else
  {
    /*
     *  We're here because of POR or hard pin reset and not EM4 wake
     *  exclusively.  This also means the LED needs to resume the
     *  initial ON state (which actually means the GPIO needs to be
     *  driven low because of the way the RGB LEDs are connected on
     *  the EFM32GG11 Starter Kit.
     */
    wakestate = false;

    pinstate = 0;
    RMU_UserResetStateSet(pinstate);
  }

  /*
   *  Set the LED to the appropriate state based on the code above that
   *  determined the reset source.
   */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, pinstate);

  /*
   * At this point, we've handled whatever initialization must be
   * performed according to the type of reset, so it's time to clear
   * RSTCAUSE so we know which reset matters next time around.
   */
  RMU_ResetCauseClear();

  /*
   * Care must be taken when unlatching pins if full pin retention is
   * used in EM4.  For this example, the only pin we care about is the
   * GPIO driving LED0, and we've already prepared to drive to the new
   * state in the previous code.
   *
   * Note that the call to GPIO_PinModeSet() above does not actually
   * change the pin state if we have had an EM4 wake-up with full
   * retention.  All we're doing is putting the new register value in
   * place so that when we do unlatch, it takes effect.
   *
   * Why mention all of this here?  Wake from EM4 not only has
   * implications if full GPIO retention is used but also with regards
   * to certain CMU and EMU registers.  Because we're about to
   * initialize the LF clocks, it will be necessary to unlatch before
   * certain register writes can take place.  The parameter passed to
   * lfClkInit() lets the function know whether or not unlatch is
   * necessary.
   *
   * In the case of our GPIO pin above, its state will have already
   * changed if our reset is not an EM4 wake (e.g. POR or pin reset).
   * However, in the case of a wake-up, the state changes when
   * EMU_UnlatchPinRetention() is called in lfClkInit().
   */
  lfClkInit(wakestate);

  /*
   * Initialize and start the RTCC.  Note that to get a 5-second wake
   * interval, the match needs to occur at 4 counts because there is
   * a 1 CNT delay from when the match occurs to when it is recognized
   * by the EM4 wake-up logic.
   */
  rtccInit(4);

  /*
   * When developing/debugging code that enters EM4 is it imperative to
   * have an "escape hatch" type mechanism, e.g. a way to pause the
   * device so that a debugger can connect in order to erase flash,
   * among other things.
   *
   * This is necessary because once a device enters EM4, it is not
   * possible to connect with a debugger.  While it is possible to
   * use the AAP in such a case to force a device erase, the timing to
   * do so is very tight and cannot be met using the normal J-Link
   * commands.  Instead, it would be necessary to use an EFM32 Starter
   * Kit configured to operate in Debug Out mode as the debug interface
   * and then to use Simplicity Commander to unlock the device.
   *
   * Before proceeding with this example, make sure PB0 is not pressed.
   * If the PB0 pin is low, turn on LED0 and LED1 and execute the BKPT
   * instruction to stop the processor in EM0 and allow a debug
   * connection to be made.
   */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0)
  {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
    __BKPT(1);
  }
  // Pin not asserted, so disable input
  else
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);

  /*
   * Configure EM4 options.  Note that this has nothing to do with the
   * selection of wake-up events.  The only things specified here are
   * the EM4 state (hibernate or shutdown, retained oscillators, pin
   * retention, and voltage scaling).
   */
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  em4Init.pinRetentionMode = emuPinRetentionLatch;      // Retain pins through wake-up
  em4Init.retainLfxo = true;

  EMU_EM4Init(&em4Init);

  EMU_EnterEM4();

  // The CPU should never get here
  __BKPT(0);
}
