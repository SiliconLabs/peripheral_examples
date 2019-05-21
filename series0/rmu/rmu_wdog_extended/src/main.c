/**************************************************************************//**
 * @file
 * @brief This project showcases the extended reset capability of the EFM32 device. 
 * In the extended reset mode, after a soft reset, everything is reset with the 
 * exception of Cryotimer, Debugger and parts of CMU, RMU and EMU. This example 
 * demonstrates the extended reset capability using pin reset and cryotimer. See 
 * readme.txt for details.
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
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_cryotimer.h"
#include "retargetserial.h"

// Global Variables 
volatile uint32_t msTicks; // counts 1ms timeTicks 
uint32_t resetCause;

// Function Declarations 
void initCMU(void);
void startCRYO(void);

// Defines 
#define COUNT_MAX			0xFFFF

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       // increment counter necessary in Delay()
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata 
  CHIP_Init();
  // Init DCDC regulator
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  RMU_ResetControl(rmuResetPin, rmuResetModeExtended);

  // Initialize the clock
  initCMU();

  // Start the Cryotimer
  startCRYO();

  // Initialize USART, enable board controller VCOM function, map LF to CRLF 
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  // Get reset cause
  resetCause = RMU_ResetCauseGet();
  // Clear all reset flags
  RMU_ResetCauseClear();

  // Print if the reset was caused by the WDOG timer
  if (resetCause & RMU_RSTCAUSE_EXTRST)
  {
    printf(" -------- PIN RESET --------\r\n");
  }

  // Setup SysTick Timer for 1 msec interrupts  
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

  while (1)
  {
    // Print out RTC clock here
    printf("Cryotimer count: %x\n", (unsigned int)CRYOTIMER_CounterGet());
    Delay(1000);
    if(CRYOTIMER_CounterGet() > COUNT_MAX)
    {
      // Restart the CNT value 
      CRYOTIMER_Enable(false);
      // Start the counter at 0 
      CRYOTIMER_Enable(true);
    }
  }
}

/**************************************************************************//**
 * @brief  Start HFXO (required for Limited reset mode)
 * Starts the high frequency external oscillator (HFXO)
 *****************************************************************************/
void initCMU(void)
{
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  // Initialize HFXO
  CMU_HFXOInit(&hfxoInit);
  // Switch HFCLK to use HFXO Crystal Oscillator input source
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // RC source HFRCO not need as we're now using the Xstal
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
}

/**************************************************************************//**
 * @brief  Initialize and Start Cryotimer
 *****************************************************************************/
void startCRYO(void)
{
  // Init Cryotimer with specific parameters 
  CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;

  // Enable clock to CRYOTIMER module 
  CMU_ClockEnable(cmuClock_CRYOTIMER ,true);

  // Configure the CRYOTIMER to use the ULFRCO which is running at 1 KHz
  init.debugRun = true;
  init.osc = cryotimerOscULFRCO;
  init.presc = cryotimerPresc_1;
  init.period = cryotimerPeriod_1k;

  CRYOTIMER_Init(&init);

  // Enable the Cryotimer
  CRYOTIMER_Enable(true);
}

