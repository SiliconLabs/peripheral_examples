/***************************************************************************//**
 * @file
 * @brief LESENSE multi channel demo for EFM32GG11. This project initializes four
 *        LESENSE channels and scans through all of them. Whenever a positive edge
 *        is detected on one of the channels, an interrupt will trigger.
 * @version 0.0.1
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2020 Silicon Labs, http://www.silabs.com</b>
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
 ******************************************************************************/

#include <stdint.h>

#include "em_device.h"
#include "em_acmp.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_core.h"
#include "em_lesense.h"

#include "bspconfig.h"
#include "bsp.h"

/***************************************************************************//**
 * Macro definitions*/
 /******************************************************************************/

  #define LESENSE_SCAN_FREQ 20  // LESENSE scan frequency set to 20Hz

/***************************************************************************//*
 * @brief  Sets up the ACMP to count LC sensor pulses
 *****************************************************************************/
void setupACMP(void)
{
  // ACMP Configuration
  ACMP_Init_TypeDef initACMP = ACMP_INIT_DEFAULT;
  initACMP.fullBias = true;  // full bias
  initACMP.biasProg = 0x1F;
  initACMP.hysteresisLevel_0 = acmpHysteresisLevel0;
  initACMP.hysteresisLevel_1 = acmpHysteresisLevel0;

  static const ACMP_VAConfig_TypeDef initVa =
  {
    acmpVAInputVDD,	// Use VDD as input for VA
    32,             // VA divider when ACMP output is 0, VDD/2
    32              // VA divider when ACMP output is 1, VDD/2
  };

  CMU_ClockEnable(cmuClock_ACMP0, true);

  // Initialize ACMP
  ACMP_Init(ACMP0, &initACMP);

  // Setup VADIV
  ACMP_VASetup(ACMP0, &initVa);

  ACMP0 -> INPUTSEL = (ACMP0 -> INPUTSEL & _ACMP_INPUTSEL_NEGSEL_MASK)
		               | ACMP_INPUTSEL_NEGSEL_VADIV;

  // Enable LESENSE control of ACMP
  ACMP_ExternalInputSelect(ACMP0, acmpExternalInputAPORT0X);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LESENSE channel 0-3 pins as input with filter enabled
  // LESENSE channel 0-3 can be mapped to either ACMP0 port or ACMP1 port
  // when only one of the ACMP is used
  GPIO_PinModeSet(gpioPortC, 0, gpioModeInputPullFilter, 0);
  GPIO_PinModeSet(gpioPortC, 1, gpioModeInputPullFilter, 0);
  GPIO_PinModeSet(gpioPortC, 4, gpioModeInputPullFilter, 0);
  GPIO_PinModeSet(gpioPortC, 5, gpioModeInputPullFilter, 0);

  //Configure LED0 for output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
}

/****************************************************************************//*
 * @brief  Sets up the LESENSE
 ******************************************************************************/
void setupLESENSE(void)
{
  LESENSE_Init_TypeDef initLesense = LESENSE_INIT_DEFAULT;
  LESENSE_ChDesc_TypeDef initLesenseCh = LESENSE_CH_CONF_DEFAULT;

  initLesense.coreCtrl.storeScanRes = false;
  
  // Enable LESENSE control of the ACMP0positive input mux
  initLesense.perCtrl.acmp0Mode = lesenseACMPModeMux;
  
  // Disable LESENSE control of ACMP1
  initLesense.perCtrl.acmp1Mode = lesenseACMPModeDisable;
  
  // Normal warm up mode
  initLesense.perCtrl.warmupMode = lesenseWarmupModeNormal;

  // Channel Configuration
  initLesenseCh.enaScanCh = true;  // Enable scan channel
  initLesenseCh.enaInt = true;
  initLesenseCh.sampleDelay = 0x2;	// Two LF Clock cycle sample delay
  initLesenseCh.sampleMode = lesenseSampleModeACMP;
  initLesenseCh.intMode = lesenseSetIntPosEdge;

  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockEnable(cmuClock_LESENSE, true);

  //Initialize LESENSE interface
  LESENSE_Init(&initLesense, true);

  // Configure channel 0, 1, 2, 3
  LESENSE_ChannelConfig(&initLesenseCh, 0);
  LESENSE_ChannelConfig(&initLesenseCh, 1);
  LESENSE_ChannelConfig(&initLesenseCh, 4);
  LESENSE_ChannelConfig(&initLesenseCh, 5);

  // Set scan frequency to desired frequency
  LESENSE_ScanFreqSet(0, LESENSE_SCAN_FREQ);

  // Set clock divisor for LF clock
  LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_2);

  // Enable interrupt in NVIC
  NVIC_EnableIRQ(LESENSE_IRQn);

  // Start continuous scan
  LESENSE_ScanStart();
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  // Initialize DCDC for series 1 board
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit(&dcdcInit);

  // Initialize LED Driver
  BSP_LedsInit();

  // Initialize GPIO
  initGPIO();
  
  // Initialize ACMP
  setupACMP();
  
  // Initialize LESENSE
  setupLESENSE();

  while(1) {
    EMU_EnterEM2(false);        //put system into EM2 mode
  }
}

void LESENSE_IRQHandler(void)
{
  // Clear all LESENSE interrupt flag
  LESENSE_IntClear(LESENSE_IFC_CH0
		  |LESENSE_IFC_CH1
		  |LESENSE_IFC_CH4
		  |LESENSE_IFC_CH5);

  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

