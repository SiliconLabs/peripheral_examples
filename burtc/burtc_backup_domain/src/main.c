/**************************************************************************//**
 * @main.c
 * @brief This project uses the Backup Power Domain and BURTC (Backup Real Time
 * Counter) to maintain a counter value even during a main power brownout. The
 * counter is saved in the BURTC retention registers and is updated every BURTC
 * compare0 value (every second by default). The BURTC is used because it is
 * one of the real time counters still available in backup mode. This project
 * uses the LFXO for higher precision, but the ULFRCO can also be used to save
 * even more energy.
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
#include "em_emu.h"
#include "em_cmu.h"
#include "em_rmu.h"
#include "em_burtc.h"
#include "segmentlcd.h"

// Note: change this to change the retention register used to hold the counter
// that will be displayed to the LCD
#define RET_REG  0

// Note: change this to change the timeout value of the BURTC (in units of seconds)
#define BURTC_TIMEOUT_S  1

// Note: change this to change the prescaler for the BURTC
#define BURTC_PRESCALE   burtcClkDiv_128

// The frequency of the LFXO used for this example
#define LFXO_FREQUENCY   32768

/***************************************************************************//**
 * @brief
 *    Initialize the backup domain
 *
 * @details
 *    Configurations used in Backup mode need to be locked before entering EM4
 ******************************************************************************/
void initBackupDomain(void)
{
  // Enable clock to low energy modules
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Initialize backup power domain
  EMU_BUPDInit_TypeDef bupdInit = EMU_BUPDINIT_DEFAULT;
  EMU_EM4Lock(false);                       // Unlock configuration
  bupdInit.enable = true;                   // Enable backup power domain
  bupdInit.inactivePower = emuPower_MainBU; // Normal operation: current flows from main power to backup power through diode
  bupdInit.activePower = emuPower_None;     // Backup mode: No connection between backup power and main power
  bupdInit.resistor = emuRes_Res3;          // Set backup "charging" resistor
  EMU_BUPDInit(&bupdInit);

  // Wait for brown out detectors to warm up
  EMU_BUReady();

  // Initialize EM4 settings
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.vreg = true;          // Enable voltage regulator in backup mode
  em4Init.osc = emuEM4Osc_LFXO; // Configure oscillators
  em4Init.lockConfig = true;    // Lock configuration in case of brown out
  EMU_EM4Init(&em4Init);
}

/***************************************************************************//**
 * @brief
 *    Initialize BURTC
 ******************************************************************************/
void initBurtc(void)
{
  // Enable LFXO oscillator for the BURTC timer
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  // Initialize BURTC
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.mode        = burtcModeEM4;    // BURTC is enabled in EM0-EM4
  burtcInit.clkSel      = burtcClkSelLFXO; // Select LFXO as clock source
  burtcInit.clkDiv      = BURTC_PRESCALE;  // Set clock divider
  burtcInit.compare0Top = true;            // Reset the timer count upon hitting compare 0 top value
  BURTC_Init(&burtcInit);

  // Set the top value for the BURTC timer
  BURTC_CompareSet(0, LFXO_FREQUENCY / BURTC_PRESCALE * BURTC_TIMEOUT_S);
}

/***************************************************************************//**
* @brief
*     BURTC IRQ Handler
******************************************************************************/
void BURTC_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = BURTC_IntGet();
  BURTC_IntClear(flags);

  // BURTC_COMP0 interrupt: update the counter in the retention register
  if (flags & BURTC_IF_COMP0) {
    uint32_t counter = BURTC_RetRegGet(RET_REG);
    BURTC_RetRegSet(RET_REG, counter + 1);
  }
}

/***************************************************************************//**
* @brief
*    Main function
*
* @details
*    If the system did enter backup mode, re-initialization of the BURTC and
*    clearing the retention register counter will be skipped
*
* @note
*    In order to access any BURTC registers, one must first call
*    RMU_ResetControl(rmuResetBU, rmuResetModeClear);
*
* @note
*    NVIC interrupts for the BURTC are enabled here instead of initBurtc()
*    because the NVIC registers are not retained upon reset and initBurtc() is
*    only called once upon normal startup. Therefore, we must make sure to call
*    NVIC_EnableIRQ(BURTC_IRQn) every time. Only BURTC registers can be
*    initialized once. The other registers will be reset upon brownout so
*    they are initialized every time
*
* @note
*    BURTC_IntEnable(BURTC_IF_COMP0) is called here simply for code neatness.
*    It could be moved into initBurtc()
*
* @note
*    BURTC_IntClear(BURTC_IF_COMP0) is called here to clear pending interrupts
*    that might have resulted from brownout. Without this line here, you will
*    see the counter skip values upon returning from brownout.
******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Read and clear RMU->RSTCAUSE as early as possible
  uint32_t resetCause = RMU->RSTCAUSE;
  RMU_ResetCauseClear();

  // Initialization
  initBackupDomain();
  SegmentLCD_Init(false);
  RMU_ResetControl(rmuResetBU, rmuResetModeClear); // Enable access to BURTC registers

  // If the system did not enter backup mode (i.e. normal startup):
  if (!(resetCause & RMU_RSTCAUSE_BUMODERST)) {
    initBurtc();                 // Initialize the BURTC
    BURTC_RetRegSet(RET_REG, 0); // Clear the counter
  }

  // Enable BURTC interrupts
  BURTC_IntClear(BURTC_IF_COMP0);
  BURTC_IntEnable(BURTC_IF_COMP0);
  NVIC_EnableIRQ(BURTC_IRQn);

  while (1) {
    SegmentLCD_Number(BURTC_RetRegGet(RET_REG)); // Update the LCD display
    EMU_EnterEM2(true);                          // Sleep while waiting for interrupt
  }
}

