/**************************************************************************//**
 * @main.c
 * @brief This project uses the BURTC (Backup Real Time Counter) to wake the
 * board up from EM4 mode and thus trigger a reset. This project also shows
 * how to use the BURTC retention registers to have data persist between
 * resets. The LCD will show the number of resets triggered by the BURTC.
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
#include "em_burtc.h"
#include "em_rmu.h"
#include "segmentlcd.h"

// Note: change this to change the retention register used to hold the counter
// that will be displayed to the LCD
#define RET_REG  0

// Note: change this to change the timeout value of the BURTC (in units of seconds)
#define BURTC_TIMEOUT_S   0.5

// The frequency of the ULFRCO used for this example
#define ULFRCO_FREQUENCY  1000

// System ms tick counter
static uint32_t volatile msTicks = 0;

/**************************************************************************//**
 * @brief
 *    Triggered every ms
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks += 1; // Increment counter used in delay()
}

/**************************************************************************//**
 * @brief
 *    Busy wait for the specified number of milliseconds
 *
 * @param [in] msDelay
 *    Number of milliseconds to delay
 *****************************************************************************/
void delay(uint32_t msDelay)
{
  uint32_t curTicks = msTicks;
  while ((msTicks - curTicks) < msDelay) {}
}

/**************************************************************************//**
 * @brief
 *    Configure SysTick to interrupt every millisecond (necessary for Delay())
 *****************************************************************************/
void initSysTick(void)
{
  while (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {}
}

/**************************************************************************//**
 * @brief
 *    Configure EM4 settings
 *****************************************************************************/
void initEm4(void)
{
  // Enable clock for accessing low energy modules
  CMU_ClockEnable(cmuClock_HFLE, true);

  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.lockConfig    = true;             // Lock regulator, oscillator and Brown Out Detection configuration.
                                            // This needs to be set when using the voltage regulator in EM4
  em4Init.osc           = emuEM4Osc_ULFRCO; // Select ULFRCO
  em4Init.buRtcWakeup   = true;             // BURTC compare or overflow will generate reset
  em4Init.vreg          = true;             // Enable voltage regulator. Needed for BURTC
  EMU_EM4Init(&em4Init);
}

/**************************************************************************//**
 * @brief
 *    Configure BURTC to count up and trigger a wakeup in EM4
 *
 * @details
 *    The COMP0 interrupt flag will trigger a reset because BURTC wakeup is set
 *    to true in the EM4 settings (see initEm4())
 *****************************************************************************/
void initBurtc(void)
{
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.mode      = burtcModeEM4;      // BURTC is enabled in EM0-EM4
  burtcInit.clkSel    = burtcClkSelULFRCO; // Select ULFRCO as clock source
  burtcInit.clkDiv    = burtcClkDiv_2;     // Choose 1kHz ULFRCO clock frequency
  BURTC_CompareSet(0, ULFRCO_FREQUENCY * BURTC_TIMEOUT_S);   // Set top value for comparator
  BURTC_IntEnable(BURTC_IF_COMP0);         // Enable compare interrupt flag
  BURTC_Init(&burtcInit);
}

/**************************************************************************//**
 * @brief
 *    Display wakeup information to the LCD
 *
 * @details
 *    Retention register #0 is being used to keep track of the number of resets
 *****************************************************************************/
void displayWakeupInfo(void)
{
  // Enable access to BURTC registers
  RMU_ResetControl(rmuResetBU, rmuResetModeClear);
  BURTC_Enable(true);

  // Get the cause of reset and clear the flags
  uint32_t volatile resetCause = RMU->RSTCAUSE;
  RMU_ResetCauseClear();

  // External reset triggered
  if (resetCause & RMU_RSTCAUSE_EXTRST) {
    SegmentLCD_Write("RESET");
    BURTC_RetRegSet(RET_REG, 0); // Reset the counter
  }
  // Reset caused by wakeup from EM4 (due to BURTC)
  else if (resetCause & RMU_RSTCAUSE_EM4RST) {
    BURTC->RET[0].REG += 1;               // Increment the counter
    SegmentLCD_Number(BURTC->RET[0].REG); // Display reset counter to the LCD
  }
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Initialization
  CHIP_Init();
  initEm4();
  SegmentLCD_Init(false);

  // Display wakeup information to the LCD for 500 milliseconds
  displayWakeupInfo();
  initSysTick(); // SysTick is used to time the delay
  delay(500);

  // Initialize the BURTC module and timer to wake us up from EM4
  initBurtc();

  // Wait for the BURTC to wake us up
  EMU_EnterEM4();
}

