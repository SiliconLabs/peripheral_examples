/**************************************************************************//**
 * @main.c
 * @brief This project shows how to use the Cryotimer with the ULFRCO to wake
 * up from EM4. This project also shows how to write/erase the USERDATA page.
 * Results of this project are shown using LEDs.
 *
 * Successful operation will show the following:
 * 1. LED0 will turn on for two seconds
 * 2. Both LEDs will turn off for two seconds
 * 3. LED1 will turn on for two seconds
 * 4. Both LEDs will turn off for two seconds
 * 5. Go back to step 3
 *
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
#include "em_msc.h"

// Note: this isn't necessary (just a convenient macro for type-casting the pointer to USERDATA_BASE)
#define USERDATA ((uint32_t*)USERDATA_BASE)

// Note: change this to one of the defined periods in em_cryotimer.h
// Wakeup events occur every 2048 prescaled clock cycles
#define CRYOTIMER_PERIOD    cryotimerPeriod_2k

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
 *    incorrect timing if not cleared.
 *
 * @note
 *    No need to enable the ULFRCO since it is always on and cannot be shut off
 *    under software control. The ULFRCO is used in this example because it is
 *    the only oscillator capable of running in EM3.
 *****************************************************************************/
void initCryotimer(void)
{
  // Enable cryotimer clock
  CMU_ClockEnable(cmuClock_CRYOTIMER, true);

  // Initialize cryotimer
  CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;
  init.osc       = cryotimerOscULFRCO; // Use the ULFRCO
  init.em4Wakeup = true;               // Enable EM4 wakeup upon triggering a Cryotimer interrupt
  init.presc     = CRYOTIMER_PRESCALE; // Set the prescaler
  init.period    = CRYOTIMER_PERIOD;   // Set when wakeup events occur
  init.enable    = false;              // Reset the Cryotimer
  CRYOTIMER_Init(&init);

  // Enable Cryotimer interrupts
  CRYOTIMER_IntEnable(CRYOTIMER_IF_PERIOD);
  NVIC_EnableIRQ(CRYOTIMER_IRQn);

  // Start the Cryotimer
  CRYOTIMER_Enable(true);
}

/**************************************************************************//**
 * @brief
 *    Initialize LED0 GPIO pin
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);
}

/**************************************************************************//**
 * @brief
 *    Initialize EM4 mode
 *
 * @note
 *    The ULFRCO must be retained in EM4 because it is the oscillator being used
 *    to power the Cryotimer. Without retaining its settings, there will be
 *    nothing to wake us up from EM4. In EM4 Shutoff mode, the ULFRCO is
 *    automatically disabled, so we must explicitly set the ULFRCO to be
 *    retained.
 *****************************************************************************/
void initEm4(void)
{
  // Configure EM4 behavior
  EMU_EM4Init_TypeDef init = EMU_EM4INIT_DEFAULT;
  init.em4State = emuEM4Hibernate; // Choose emuEM4Shutoff or emuEM4Hibernate
  init.retainUlfrco = true;        // Make sure the ULFRCO is still powering the Cryotimer in EM4
  init.pinRetentionMode = emuPinRetentionDisable; // No GPIO retention
  EMU_EM4Init(&init);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @note
 *    The only reason this program chooses to wait in EM3 is to show the user
 *    the state of the LEDs and therefore show that the reset cause is being
 *    appropriately detected. Without the EMU_EnterEM3(false) line, the board
 *    would immediately go into EM4 and thus turn off the LEDs before the user
 *    could see them turn on.
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

  // Initialization
  initGpio(); // Enable GPIO clock
  initEm4();  // Configure EM4 settings

  // Determine the reset cause
  if ((resetCause & RMU_RSTCAUSE_PORST)           // Reset from powering on the board
        || (resetCause & RMU_RSTCAUSE_EXTRST)     // Reset from an external pin
        || (resetCause & RMU_RSTCAUSE_SYSREQRST)) // Reset from a system request (e.g. from the debugger)
  {
    // Initialize the state of the LEDs
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0); // Turn LED0 off
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0); // Turn LED1 off

    // Erase the USERDATA page and then write an arbitrary value to it
    uint32_t value = 32;
    MSC_Init();
    if (MSC_ErasePage(USERDATA) != mscReturnOk) {
      while (1);
    }
    if (MSC_WriteWord((USERDATA + 4), &value, 4) != mscReturnOk) {
      while (1);
    }
  }
  else if (resetCause & RMU_RSTCAUSE_EM4RST) // Reset from waking up from EM4
  {
    // Note: We don't really need to call this function since the MSC is unlocked by default upon reset.
    //       It's just good practice to call this before trying to write to the flash memory
    MSC_Init();

    // Clear the USERDATA page of any previous data stored
    if (MSC_ErasePage(USERDATA) != mscReturnOk) {
      while (1);
    }

    // Read the written data from the flash location it was stored in
    uint32_t userValue = USERDATA[4];

    // Check if the page was erased (erasing flash memory sets all of the bits to 1
    if (userValue == 0xFFFFFFFF) {
      GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1); // Turn LED1 on
    }

    // Check status flags and get stuck in a while loop if any errors occurred
    if (MSC->STATUS & MSC_STATUS_BUSY) {
      while (1);
    } else if (MSC->STATUS & MSC_STATUS_LOCKED) {
      while (1);
    } else if (MSC->STATUS & MSC_STATUS_INVADDR) {
      while (1);
    } else if (MSC->STATUS & MSC_STATUS_WORDTIMEOUT) {
      while (1);
    } else if (MSC->STATUS & MSC_STATUS_ERASEABORTED) {
      while (1);
    } else if ((MSC->STATUS & _MSC_STATUS_PWRUPCKBDFAILCOUNT_MASK) > 0) {
      while (1);
    }
  }

  // Check if the data was written to the USERDATA page
  // Note: this will only occur once since after waking up from EM4 we clear the USERDATA page
  uint32_t userValue = USERDATA[4];
  if (userValue == 32) {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1); // Turn LED0 on
  }

  // Initialize and start Cryotimer to run for CRYOTIMER_PERIOD ticks (2 seconds by default)
  initCryotimer();

  // This is only here to show that the LED0 turns on since without this line we would immediately
  // go into EM4 and thus lose the state of the GPIO pins (i.e. LED0 will turn off)
  EMU_EnterEM3(false);

  // Go into EM4 and wait for Cryotimer wakeup
  // At this point, both LEDs will be off since there is no GPIO retention
  EMU_EnterEM4();
}


