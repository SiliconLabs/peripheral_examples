/***************************************************************************//**
 * @file main_rtcc_em4_wake.c
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

// Configuration Lock Word 0 (holds the bootloader enable bit)
#define CLW0 *(uint32_t *)(LOCKBITS_BASE + (122 << 2))

/*
 *  Specify the LFXO frequency in Hz.  This provides flexibility for
 *  configuring the RTCC so as to accommodate non-standard frequencies
 *  like 32 kHz at the expensive of losing the use of capture/compare
 *  channel 0.
 */
#define LFXOFREQ  32768

/**************************************************************************//**
 * @brief
 *    Initialize the LF domain clocks.
 *****************************************************************************/
void lfClkInit(bool unlatch)
{
  /*
   * A LF oscillator that is retained in EM4 cannot be selected as
   * as the LFECLK source with CMU_ClockSelectSet().  Until they are
   * unlatched, writes to the CMU registers for LFECLK domain have
   * no effect.
   *
   * This means that writing to CMU_OSCENCMD to "restart" the LFXO,
   * for example, has no effect (the LFXO remains running because
   * it was retained).  Likewise, CMU_STATUS_LFXOENS is not going to
   * change state until it is unlatched.

   * CMU_ClockSelectSet() calls CMU_OscillatorEnable() to start the
   * specified oscillator, so it will stall when called to select the
   * LFXO as the LFECLK source after wake from EM4.
   *
   * So, to properly manage this, manually select the LFXO as the
   * source of the LFECLK now; enabling the LFXO, which is handled
   * differently depending on the source of reset, is done later.
   */
  CMU->LFECLKSEL = CMU_LFECLKSEL_LFE_LFXO;

  /*
   * Enable the RTCC clock here.  LFECLKEN0 is a latched register,
   * and while CMU_ClockEnable(cmuClock_RTCC, true) does the same
   * thing, it's useful to call this out separately along with the
   * other latched CMU registers.
   */
  CMU->LFECLKEN0 = CMU_LFECLKEN0_RTCC;

  /*
   * In addition to manually selecting the LFXO as the LFECLK source,
   * reconfiguring the LFXO after wake from EM4 should be handled in
   * a slightly different fashion.
   *
   * Any LF oscillator retained in EM4 will be running upon wake-up,
   * so it's not necessary to reinitialize the oscillator as would
   * be the case after a hard reset, such as POR or pin reset.
   *
   * In the case of the LFXO, the usual start-up delay timeout period
   * of 32768 cycles (CMU_LFXOCTRL_TIMEOUT = 0x7) can be reduced to
   * just two cycles.
   *
   * After doing this, all registers that need to be "re-painted"
   * (rewritten to their pre-EM4 entry values) have been taken care
   * of, and it is now safe to unlatch.
   */
  if (unlatch)
  {
    CMU->LFXOCTRL = (CMU->LFXOCTRL & (~_CMU_LFXOCTRL_MASK))
                    | CMU_LFXOCTRL_TIMEOUT_2CYCLES;

    EMU_UnlatchPinRetention();
  }
  // For non-EM4 resets, the LFXO needs to be initialized as usual.
  else
  {
    CMU_LFXOInit_TypeDef lfxo = CMU_LFXOINIT_DEFAULT;
    CMU_LFXOInit(&lfxo);
  }

  // It is now safe to (re)start the LFXO
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
}

/**************************************************************************//**
 * @brief
 *    Initialize the RTCC.
 *****************************************************************************/
void rtccInit(uint32_t next, bool wake)
{
  /*
   *  Need full initialization if reset was other than wake from EM4.
   *  The RTCC clock was previously enabled in lfClkInit().
   */
  if (!wake)
  {
    RTCC_Init_TypeDef rtcc = RTCC_INIT_DEFAULT;
    rtcc.debugRun         = false;
    rtcc.precntWrapOnCCV0 = true;
    rtcc.cntWrapOnCCV1    = false;
    rtcc.presc            = rtccCntPresc_1;
    rtcc.prescMode        = rtccCntTickCCV0Match;
    rtcc.cntMode          = rtccCntModeNormal;
    rtcc.enaOSCFailDetect = false;

    /*
     * Set RTCC_CC0_CCV to frequency of the low-frequency crystal used
     * (minus 1).  When the pre-counter (RTCC_PRECNT) matches this
     * value, the counter (RTCC_CNT) increments by one.
     *
     * When using the PRECENT match feature, the actual comparison is
     * to RTCC_CC0_CCV bits [14:0] because the pre-counter is 15 bits
     * wide while the capture/compare value register is 32 bits wide.
     *
     * This also why LFXOFREQ - 1 is written to RTCC_CC0_CCV.  In the
     * case of a 32.768 kHz crystal, writing 32768 (0x8000) never
     * results in a match because bit 15 is not compared, thus RTCC_CNT
     * increments at the frequency of the LFECLK.
     *
     * This option is used instead of a 2^n prescaler tap selectable
     * by RTCC_CTRL_CNTPRESC in order to accommodate non-standard LFXO
     * crystal frequencies, like 32 kHz.
     *
     * Note that RTCC_ChannelCCVSet(0, LFXOFREQ - 1) can be used to set
     * the wrap-around value.  However, the direct register write is
     * used here to correspond to the reference manual RTCC chapter
     * language, which specifically describes this functionality in
     * terms of the RTCC_CTRL_CCV0MATCH bit enabling RTCC_PRECENT
     * wrap-around based on the value of RTCC_CC0_CCV.
     */
    RTCC->CC[0].CCV = LFXOFREQ - 1;

    // Initialize and start counting
    RTCC_Init(&rtcc);

    /*
     * Set up RTCC capture/compare channel 1 for match operation.
     * These events will provide the signal to wake from EM4.
     */
    RTCC_CCChConf_TypeDef compCfg = RTCC_CH_INIT_COMPARE_DEFAULT;
    RTCC_ChannelInit(1, &compCfg);
  }

  /*
   * Match at the current RTCC count plus 'next' counts, which will
   * be 'next' seconds later because RTCC_CNT increments once per
   * LFXOFREQ counts of PRECENT.
   */
  RTCC_ChannelCCVSet(1, RTCC_CounterGet() + next);

  /*
   * Enable capture/compare channel 1 match interrupt.  Note that only
   * the CC1 interrupt request is being enabled and not the NVIC RTCC
   * interrupt source.  No processor interrupt is ever going to be
   * generated.  This example sits in EM4 most of the time, and the
   * CC1 match interrupt is what wakes the device from EM4.
   */
  RTCC_IntClear(_RTCC_IF_MASK);
  RTCC_IntEnable(RTCC_IEN_CC1);
  RTCC_EM4WakeupEnable(true);

  /*
   * The last lines of code in this function are crutches of sorts for
   * this example but might also be necessary in real application code.
   *
   * 1. Check if Configuration Lock Word 0 (CLW0) bit 1 = 1.
   *
   * All EFM32TG11 devices are factory pre-programmed with the AN0003
   * bootloader.  Without a valid application in flash, the RTCC is
   * used to periodically check the state of the bootloader pins.
   *
   * When the bootloader transfers execution to the application code,
   * it restores any registers used to their reset states.  In the case
   * of the RTCC this means writing RTCC_CTRL to 0.
   *
   * However, when the RTCC is enabled in EM4, its registers are
   * retained across wake-ups (regardless of the source) and do not
   * require repainting.  Consequently, in attempting to restore the
   * device to its reset state, the bootloader disables the RTCC!
   *
   * Because the bootloader is enabled by default (CLW0 bit 1 = 1),
   * it is necessary to re-enable the RTCC when running this example
   * on a factory-fresh device or Starter Kit.
   *
   * In practice, this causes the RTCC to lose time because the
   * bootloader effectively stops it until re-enabled here.  Disable
   * the bootloader to avoid this by writing a 0 to CLW0 bit 1.  The
   * same flash programming procedure used to enable debug lock or to
   * program other lock word registers would be used to do this.  When
   * this example runs on a device with CLW0 bit 1  = 0, the RTCC
   * remains enabled across wake-ups and keeps time constantly.
   *
   * 2. Account for clock domain boundary crossing.
   *
   * Writes to RTCC registers must generally cross the clock domain
   * boundary between the peripheral registers, which are clocked by a
   * local copy of the HFBUSCLKLE, and the RTCC logic, which is clocked
   * by the LFECLK.
   *
   * While these writes do not stall the CPU (e.g. the RTCC does not
   * delay completion of the transfer pending acknowledgment of the
   * write in the LFECLK domain), they do not take effect immediately
   * as far as the CPU is concerned.  Consequently, if firmware enters
   * EM4 immediately after configuring a match event and writing to
   * RTCC_CTRL to start the counter, the chip will never wake because
   * the counter will not have started.
   *
   * For code where this might be a concern, there are a few different
   * ways to handle it.  One option would be to simply wait for the
   * first tick to register in RTCC_CNT.  This example clears the
   * RTCC_STATUS register BUMODETS bit, which requires a write to the
   * RTC_CMD_CLEARSTATUS bit.  The CMD bit in RTCC_SYNCBUSY reflects
   * the completion of this write and can be used to confirm that the
   * RTCC is configured and enabled.
   */
  uint32_t isBootloaderEnabled = CLW0;
  if (((isBootloaderEnabled >> 1) & 0x1) == 1)
    RTCC_Enable(true);

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
   * These clocks must be enabled regardless of whether the last
   * reset is courtesy of EM4 wake-up or another source.  Avoid
   * possibly redundant code by doing so here.
   */
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Get reset cause(s)
  rflags = RMU_ResetCauseGet();

  /*
   * What caused the last reset?
   *
   * For any given reset there can be multiple causes.
   *
   * Any source that causes a wake from EM4 (e.g. a GPIO pin or the
   * CRYOTIMER) sets the RMU_RSTCAUSE register's EM4RST bit.  For this
   * reason, assertion of the RESETn pin while in EM4 sets both the
   * EXTRST and EM4RST bits in RMU_RSTCAUSE.
   *
   * To simplify this example, it matters only if the last recent was
   * caused by the RTCC waking the system from EM4 and not something
   * something else.
   */
  if (rflags == RMU_RSTCAUSE_EM4RST)
  {
    /*
     * Reset was caused by EM4 wake-up, so some register repainting
     * (rewriting of registers to their states before EM4 entry) is
     * going to be necessary before performing the unlatch operation.
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
    if (pinstate == 1)
      pinstate = 0;
    else
      pinstate = 1;
  }
  /*
   * Something other than EM4 wake-up alone caused the last reset,
   * which means the RTCC is no longer keeping time and must be
   * re-initialized.
   */
  else
  {
    wakestate = false;

    // Turn LED on because GPIO was not retained.
    pinstate = 1;
  }

  // Save the pin state for the next wake-up.
  RMU_UserResetStateSet(pinstate);

  /*
   *  Set the LED to the appropriate state based on the code above that
   *  determined the reset source.
   */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, pinstate);

  /*
   * RMU_RSTCAUSE is retained between resets; it must be explicitly
   * cleared in order to determine the cause of the next reset.
   */
  RMU_ResetCauseClear();

  /*
   * This example uses GPIO retention to keep the LED on or off through
   * each EM4 entry and subsequent wake-up.  Enabling retention latches
   * the state of all GPIO pin control signals so that they are
   * unaffected by any reset source other than POR.
   *
   * Because wake from EM4 is a reset, the GPIO registers assume their
   * default states, but the pins themselves do not revert to register
   * control.  Firmware must first update the GPIO registers, including
   * repainting those for pins that are not supposed to change state,
   * and then call EMU_UnlatchPinRetention() to globally unlatch them.
   *
   * Why mention this here?  The CMU LF oscillator and LFECLK registers
   * are also latched and must be repainted on wake from EM4.  The
   * parameter passed to lfClkInit() lets the function know whether or
   * not unlatch is necessary.
   */
  lfClkInit(wakestate);

  /*
   * Setup the RTCC and start it when not waking from EM4.  Note that
   * to get a 5-second wake interval, the match needs to occur at 4
   * counts because there is a 1 count delay from when the match occurs
   * to when it is recognized by the EM4 wake-up logic.
   */
  rtccInit(4, wakestate);

  /*
   * When developing/debugging code that enters EM4 it is imperative to
   * have an "escape hatch" type of mechanism, e.g. a way to pause the
   * device so that a debugger can connect in order to erase flash,
   * among other things.
   *
   * This is necessary because once a device enters EM4, it is not
   * possible to connect with a debugger.  While the AAP cab be used in
   * such a case to force a device erase, the timing to do so is very
   * tight and cannot be met using the normal J-Link commands.  Instead,
   * an EFM32 Starter Kit must be used as the debug interface along with
   * Simplicity Commander to unlock the device.
   *
   * Before proceeding with this example, make sure PB0 is not pressed.
   * If the PB0 pin is low, turn on LED0 and LED1 and execute the BKPT
   * instruction to stop the processor in EM0 and allow a debug
   * connection to be made.
   */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0)
  {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1);
    __BKPT(1);
  }
  // Pin not asserted, so disable input
  else
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);

  /*
   * Configure EM4 options.  Note that this has nothing to do with the
   * selection of wake-up events.  The only things specified here are
   * the EM4 state (hibernate or shutdown), retained oscillators, pin
   * retention, and voltage scaling.
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
