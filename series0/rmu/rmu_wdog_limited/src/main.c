/**************************************************************************//**
 * @file
 * @brief This project showcases the Limited Reset capability of the EFM32 
 * device. In the Limited Reset mode, after a soft reset, everything is reset 
 * with the exception of Cryotimer, Debugger, RTCC, VMON and parts of CMU, RMU 
 * and EMU. This example demonstrates the Limited Reset capability using 
 * watchdog reset and RTCC. See readme.txt for details.
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
#include "em_rtcc.h"
#include "em_wdog.h"
#include "retargetserial.h"

// Global Variables 
volatile uint32_t msTicks; // counts 1ms timeTicks 
uint32_t resetCause;

// Function Declarations 
void initCMU(void);
void initRTCC(void);
void initWDOG(void);

// Defines 
#define LFRCO_FREQUENCY                 32768
#define WAKEUP_INTERVAL_MS              1000
#define RTCC_COUNT_BETWEEN_WAKEUP        (((LFRCO_FREQUENCY * WAKEUP_INTERVAL_MS) / 1000)-1)

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

  // Set the Watchdog reset to limited mode 
  RMU_ResetControl(rmuResetWdog, rmuResetModeLimited);

  // Initialize the clock 
  initCMU();
  // Initialize the RTCC 
  initRTCC();

  // Initialize USART, enable board controller VCOM function, map LF to CRLF 
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  // Store the cause of the last reset, and clear the reset cause register 
  resetCause = RMU_ResetCauseGet();
  // Clear Reset causes so we know which reset occurs the next time 
  RMU_ResetCauseClear();

  // Print if the reset was caused by the WDOG timer 
  if (resetCause & RMU_RSTCAUSE_WDOGRST)
  {
    printf(" -------- WATCHDOG RESET --------\r\n");
  }

  // Setup SysTick Timer for 1 msec interrupts  
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

  // Initialize the Watchdog 
  initWDOG();

  while (1)
  {
    // Print out RTCC count here 
    printf("RTCC count: %x\n", (unsigned int)RTCC->CNT);
    Delay(1000);
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
 * @brief  Start LFRCO for RTC/RTCC
 * Starts the low frequency RC oscillator (LFRCO) and routes it to the RTC/RTCC
 *****************************************************************************/
void startLfrcoForRtc(void)
{
  // Starting LFRCO and waiting until it is stable 
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
  // Ensure LE modules are clocked.
  CMU_ClockEnable( cmuClock_HFLE, true );
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
  // Enable RTCC module clock.
  CMU_ClockEnable( cmuClock_RTCC, true );
}

/**************************************************************************//**
 * @brief RTCC initialization
 *****************************************************************************/
void initRTCC(void)
{
  startLfrcoForRtc();
  // Initialize RTCC
  RTCC_Init_TypeDef initRTCC = RTCC_INIT_DEFAULT;
  RTCC_CCChConf_TypeDef initRTCCCompareChannel = RTCC_CH_INIT_COMPARE_DEFAULT;
  // Set clock divider.
  initRTCC.presc = rtccCntPresc_1024;
  initRTCC.cntWrapOnCCV1 = true;        // Clear counter on compare match 
  RTCC_Init( &initRTCC );
  // Set up Compare channel.
  RTCC_ChannelInit( 1, &initRTCCCompareChannel);
  RTCC_ChannelCCVSet(1, RTCC_COUNT_BETWEEN_WAKEUP);
}

/**************************************************************************//**
 * @brief Watchdog initialization
 *****************************************************************************/
void initWDOG(void)
{
  // Enabling clock to the interface of the low energy modules (including the Watchdog)
  CMU_ClockEnable(cmuClock_HFLE, true);

  WDOG_Init_TypeDef wdogInit = WDOG_INIT_DEFAULT;
  wdogInit.perSel = wdogPeriod_8k; // 8193 clock cycles of a 1kHz clock  ~8 seconds period
  // Initializing watchdog with chosen settings 
  WDOG_Init(&wdogInit);
  // Enbale the WDOG 
  WDOG_Enable(true);
}
