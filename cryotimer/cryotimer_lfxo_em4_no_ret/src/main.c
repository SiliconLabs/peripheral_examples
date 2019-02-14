/**************************************************************************//**
 * @main.c
 * @brief This project shows how to use the Cryotimer in EM4 with the LFXO and
 * no GPIO retention. This project does so by using LEDs. For the exact program
 * flow, please see the readme.txt.
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
#include "em_cryotimer.h"
#include "em_rmu.h"
#include "bsp.h"

// Note: change this to one of the defined periods in em_cryotimer.h
// Wakeup events occur every 65536 prescaled clock cycles
#define CRYOTIMER_PERIOD    cryotimerPeriod_64k

// Note: change this to one of the defined prescalers in em_cryotimer.h
// The clock is divided by one
#define CRYOTIMER_PRESCALE  cryotimerPresc_1

/**************************************************************************//**
 * @brief
 *    Cryotimer interrupt service routine
 *
 * @note
 *    The barrier below is make sure the interrupt flags get cleared before
 *    continuing. This ensures that the interrupt is not retriggered before
 *    exiting the Handler. A barrier like this is generally required when the
 *    peripheral clock is much slower than the CPU core clock.
 *
 * @note
 *    Despite the note above, the barrier is just a precaution. The code
 *    might work without it as well.
 *****************************************************************************/
void CRYOTIMER_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = CRYOTIMER_IntGet();
  CRYOTIMER_IntClear(flags);

  // Put a barrier here to ensure interrupts are not retriggered. See note above
  __DSB();
}

/**************************************************************************//**
 * @brief
 *    Initialize Cryotimer
 *
 * @details
 *    The Cryotimer is reset in order to clear the CNT register. This register
 *    still holds its value from before the EM4 reset and therefore gives
 *    incorrect timing if not cleared. The timer will be started later in the
 *    main().
 *****************************************************************************/
void initCryotimer(void)
{
  // Enable cryotimer clock
  CMU_ClockEnable(cmuClock_CRYOTIMER, true);

  // Initialize cryotimer
  CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;
  init.osc       = cryotimerOscLFXO;   // Use the LFXO
  init.em4Wakeup = true;               // Enable EM4 wakeup upon triggering a Cryotimer interrupt
  init.presc     = CRYOTIMER_PRESCALE; // Set the prescaler
  init.period    = CRYOTIMER_PERIOD;   // Set when wakeup events occur
  init.enable    = false;              // Reset the Cryotimer and don't start the timer
  CRYOTIMER_Init(&init);

  // Enable Cryotimer interrupts
  CRYOTIMER_IntEnable(CRYOTIMER_IF_PERIOD);
  NVIC_EnableIRQ(CRYOTIMER_IRQn);
}

/**************************************************************************//**
 * @brief
 *    Initialize EM4 mode
 *
 * @note
 *    The LFXO must be retained in EM4 because it is the oscillator being used
 *    to power the Cryotimer. Without retaining its settings, there will be
 *    nothing to wake us up from EM4. In EM4 Shutoff mode, the LFXO is
 *    automatically disabled, so we must explicitly set the LFXO to be
 *    retained.
 *
 * @note
 *    The LFXO must be enabled before setting the RETAINLFXO bit in the
 *    EMU_EM4CTRL register.
 *****************************************************************************/
void initEm4(void)
{
  // Configure EM4 behavior
  EMU_EM4Init_TypeDef init = EMU_EM4INIT_DEFAULT;
  init.em4State = emuEM4Shutoff; // emuEM4Hibernate also works
  init.retainLfxo = true;        // Make sure the LFXO is still powering the Cryotimer in EM4
  init.pinRetentionMode = emuPinRetentionDisable; // No GPIO retention
  EMU_EM4Init(&init);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    The only reason this program chooses to wait in EM2 is to show the user
 *    the state of the LEDs and therefore show that the reset cause is being
 *    appropriately detected. Without the EMU_EnterEM2(false) line, the board
 *    would immediately go into EM4 and thus turn off the LEDs before the user
 *    could see them turn on.
 *
 * @details
 *    If the startup delay for the LFXO is not shortened, then enabling the LFXO
 *    oscillator will take 32768 clock cycles (about 1 second).
 *
 * @note
 *    The LFXO must be enabled before setting the RETAINLFXO bit in the
 *    EMU_EM4CTRL register.
 *
 * @note
 *    It is good practice to always call EMU_UnlatchPinRetention() when
 *    dealing with EM4 (even if you aren't using retention for the GPIO
 *    pins). The reason is because the registers will have contradictory values
 *    to the latched values upon waking up from EM4. To ensure a consistent
 *    reset state, the unlatch command should be given after properly
 *    reconfiguring these latched registers so that the new values can be
 *    re-latched upon entering EM4. Another good reason to always call
 *    EMU_UnlatchPinRetention() is because of the "EM4H I/O Retention Cannot Be
 *    Disabled" Errata (see the errata for your particular board).
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Get the reset cause and also clear the reset cause register because it can
  // only be cleared by software.
  uint32_t resetCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  // GPIO Initialization
  CMU_ClockEnable(cmuClock_GPIO, true); // Enable GPIO clock
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1); // Turn LED0 on

  // Determine the reset cause
  if ((resetCause & RMU_RSTCAUSE_PORST)           // Reset from powering on the board
        || (resetCause & RMU_RSTCAUSE_EXTRST)     // Reset from an external pin
        || (resetCause & RMU_RSTCAUSE_SYSREQRST)) // Reset from a system request (e.g. from the debugger)
  {
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0); // Turn LED1 off
  }
  // If reset was caused by waking up from EM4
  else if (resetCause & RMU_RSTCAUSE_EM4RST) {

    // Turn LED1 on
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1);

    // Since the LFXO has been retained in EM4, we can shorten the startup
    // delay to 2 cycles when re-enabling the LFXO after EM4 wakeup
    CMU->LFXOCTRL = (CMU->LFXOCTRL
                      & (~_CMU_LFXOCTRL_MASK))
                      | CMU_LFXOCTRL_TIMEOUT_2CYCLES;

    // Unlatch the registers that were latched upon entering EM4
    EMU_UnlatchPinRetention();
  }

  // The LFXO oscillator must be enabled before calling initEm4()
  // This takes about 1 second if the LFXO startup delay is not shortened
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  // Initialize the Cryotimer and EM4 settings
  initCryotimer();
  initEm4();

  // Start the Cryotimer
  CRYOTIMER_Enable(true);

  // Go into EM2 and wait for Cryotimer IRQ
  // At this point, one of two cases could occur
  // 1) LED0 will be on (this case only occurs once upon initially resetting the board)
  // 2) LED0 and LED1 will be on
  EMU_EnterEM2(false);

  // Go into EM4 and wait for Cryotimer wakeup
  // At this point, both LEDs will be off since there is no GPIO retention
  // (i.e. the GPIO pins used for the LEDs will both be reset to zero)
  EMU_EnterEM4();
}

