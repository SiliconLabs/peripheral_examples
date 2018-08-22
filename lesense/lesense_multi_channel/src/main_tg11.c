/***************************************************************************//**
 * @file
 * @brief LESENSE multi channel demo for EFM32TG11. This project initializes four
 *        LESENSE channels and scans through all of them. Whenever a positive edge
 *        is detected on one of the channels, an interrupt will trigger.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "em_device.h"
#include "em_acmp.h"
#include "em_assert.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_core.h"
#include "em_lesense.h"
#include "em_pcnt.h"

#include "bspconfig.h"
#include "bsp.h"
#include "bsp_trace.h"

/***************************************************************************//**
 * Macro definitions*/
 /******************************************************************************/

  #define LESENSE_SCAN_FREQ 20

/***************************************************************************//**
 * Global variables*/
 /******************************************************************************/

  volatile uint32_t setflag;	//set flag to toggle the LED on GG11

/***************************************************************************//**/
 /* Prototypes*/
 /******************************************************************************/

/************************************************************************************//*
 * @brief  Sets up the ACMP to count LC sensor pulses
 **************************************************************************************/
static void setupACMP(void)
{
  // ACMP configuration constant table.
	static const ACMP_Init_TypeDef initACMP =
	{
	  .fullBias                 = true,                  // fullBias
	  .biasProg                 = 0x1F,                  // biasProg
	  .interruptOnFallingEdge   = false,                 // interrupt on rising edge
	  .interruptOnRisingEdge    = false,                 // interrupt on falling edge
	  .inputRange               = acmpInputRangeFull,    // Full ACMP rang
	  .accuracy                 = acmpAccuracyHigh,      // High accuracy
	  .powerSource              = acmpPowerSourceAvdd,   // Use AVDD as power source, default to 3.3V
	  .hysteresisLevel_0        = acmpHysteresisLevel0,  // hysteresis level 0
	  .hysteresisLevel_1        = acmpHysteresisLevel0,  // hysteresis level 1
	  .vlpInput                 = acmpVLPInputVADIV,     // Use VADIV as the VLP input source.
	  .inactiveValue            = false,                 // no inactive value
	  .enable                   = true                   // Enable after init
	};

	static const ACMP_VAConfig_TypeDef initVa =
	{
	  acmpVAInputVDD,                                    // Use VDD as input for VA
	  32,                                                // VA divider when ACMP output is 0, VDD/2
	  32                                                 // VA divider when ACMP output is 1, VDD/2
	};

	CMU_ClockEnable(cmuClock_ACMP1, true);

	//Initialize ACMP
	ACMP_Init(ACMP1, &initACMP);

	//Setup VADIV
	ACMP_VASetup(ACMP1, &initVa);

	// Setup ACMP0 CH0
	ACMP_ChannelSet(ACMP1, acmpInputVADIV, acmpInputAPORT0XCH0);

	// Setup ACMP0 CH1
	ACMP_ChannelSet(ACMP1, acmpInputVADIV, acmpInputAPORT0XCH1);

	// Setup ACMP0 CH2
	ACMP_ChannelSet(ACMP1, acmpInputVADIV, acmpInputAPORT0XCH2);

	// Setup ACMP0 CH3
	ACMP_ChannelSet(ACMP1, acmpInputVADIV, acmpInputAPORT0XCH3);

	//Enable LESENSE control of ACMP
	ACMP_ExternalInputSelect(ACMP1, acmpExternalInputAPORT0X);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LESENSE channel 0-3 pins as input with filter enabled
  // LESENSE channel 0-3 can be mapped to either ACMP0 port or ACMP1 port when only one of the ACMP is used
  GPIO_PinModeSet(gpioPortC, 8, gpioModeInputPullFilter, 0);
  GPIO_PinModeSet(gpioPortC, 9, gpioModeInputPullFilter, 0);
  GPIO_PinModeSet(gpioPortC, 10, gpioModeInputPullFilter, 0);
  GPIO_PinModeSet(gpioPortC, 11, gpioModeInputPullFilter, 0);

  // Configure LED0 for output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
}

/**********************************************************************************************//*
 * @brief  Sets up the LESENSE
 ************************************************************************************************/
static void setupLESENSE(void)
{
  // LESENSE configuration structure
  static const LESENSE_Init_TypeDef initLesense =
  {
    .coreCtrl         =
    {
       .scanStart    = lesenseScanStartPeriodic,                // set scan to periodic scan
	   .prsSel       = lesensePRSCh0,                           // PRS selection channel 0
	   .scanConfSel  = lesenseScanConfDirMap,                   // direct scan configuration register usage
	   .invACMP0     = false,                                   // no invert ACMP0
	   .invACMP1     = false,                                   // no invert ACMP1
	   .dualSample   = false,                                   // no dualSample
	   .storeScanRes = false,                                   // do not Store SCANERS in RAM after each scan
	   .bufOverWr    = true,                                    // always write to buffer even if it is full
	   .bufTrigLevel = lesenseBufTrigHalf,                      // set DMA and interrupt flag when buffer is half full
	   .wakeupOnDMA  = lesenseDMAWakeUpDisable,                 // Disable DMA wakeup
	   .biasMode     = lesenseBiasModeDutyCycle,                // Duty cycle bias between low power and high accuracy mode
	   .debugRun     = false                                    // LESENSE not running on debugging mode
    },

	.timeCtrl          =
	{
      .startDelay      = 0
	},

	.perCtrl           =
	{
	  .acmp0Mode       = lesenseACMPModeDisable,                // Disable ACMP0
	  .acmp1Mode       = lesenseACMPModeMux,                    // Enable ACMP1 Mux
	  .warmupMode      = lesenseWarmupModeNormal                // ACMP Warmup normal
	},
  };

  // Channel configuration
  static const LESENSE_ChDesc_TypeDef initLesenseCh =
  {
    .enaScanCh     = true,
	.enaPin        = true,
	.enaInt        = true,
	.chPinExMode   = lesenseChPinExDis,		//Disable excitation
	.chPinIdleMode = lesenseChPinIdleDis,	//Disable idle
	.useAltEx      = false,
	.shiftRes      = false,
	.invRes        = false,
	.storeCntRes   = true,
	.exClk         = lesenseClkHF,
	.sampleClk     = lesenseClkLF,
	.exTime        = 0x07,
	.sampleDelay   = 0x0A,
	.measDelay     = 0x00,
	.acmpThres     = 0x00,
	.sampleMode    = lesenseSampleModeACMP,		//Sample directly from ACMP
	.intMode       = lesenseSetIntPosEdge,		//Interrupt on positive edge
	.cntThres      = 0x0000,
	.compMode      = lesenseCompModeLess,
  };

  // Use LFXO as LESENSE clock source since it is already used by the RTCC
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockEnable(cmuClock_LESENSE, true);

  //Initialize LESENSE interface _with_ RESET
  LESENSE_Init(&initLesense, true);

  // Configure channel 0, 1, 2, 3
  LESENSE_ChannelConfig(&initLesenseCh, 0);
  LESENSE_ChannelConfig(&initLesenseCh, 1);
  LESENSE_ChannelConfig(&initLesenseCh, 2);
  LESENSE_ChannelConfig(&initLesenseCh, 3);

  // Set scan frequency to 20hz
  LESENSE_ScanFreqSet(0, LESENSE_SCAN_FREQ);

  // Set clock divisor for LF clock
  LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_2);
  // Set clock divisor for HF clock
  LESENSE_ClkDivSet(lesenseClkHF, lesenseClkDiv_1);

  //Enable interrupt in NVIC
  NVIC_EnableIRQ(LESENSE_IRQn);

  // Start continuous scan
  LESENSE_ScanStart();
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /*Initialize DCDC for series 1 board*/
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit(&dcdcInit);

  // Set SysTick Timer for 1 msec interrupts
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE)/1000))while(1);

  /* Initialize GPIO */
  initGPIO();
  /* Initialize ACMP */
  setupACMP();
  /*Initialize LESENSE*/
  setupLESENSE();

  while(1) {
    EMU_EnterEM2(false);  //put system into EM2 mode
  }
}

void LESENSE_IRQHandler(void)
{
  /* Clear all LESENSE interrupt flag */
  LESENSE_IntClear(LESENSE_IFC_CH0
		  |LESENSE_IFC_CH1
		  |LESENSE_IFC_CH2
		  |LESENSE_IFC_CH3);

  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

