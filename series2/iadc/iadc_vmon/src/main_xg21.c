/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the usage of the window comparator feature
 * in the IADC to monitor supply voltage for xG21 devices.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
 
#include "stdio.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_iadc.h"
#include "em_gpio.h"
#include "bsp.h"
#include "retargetserial.h"

/******************************************************************************
 *******************************   DEFINES   **********************************
 *****************************************************************************/
#define AVDD_VMON          1

// Set CLK_ADC to 100kHz (this corresponds to a sample rate of 10ksps)
#define CLK_SRC_ADC_FREQ        10000000  // CLK_SRC_ADC
#define CLK_ADC_FREQ            100       // CLK_ADC

#define MENU_MAX      2


/******************************************************************************
 ***************************   GLOBAL VARIABLES   *****************************
 *****************************************************************************/

// Stores latest ADC sample
static volatile IADC_Result_t sample;

static volatile bool detected_flag = false;

// Upper and lower bound for Window Comparator
// 16-bit left-justified format; 12-bit conversion result compared to upper
// 12-bits of window comparator
static uint16_t vdd_lower_bound = 0x6550;   // 1.9V
static uint16_t vdd_upper_bound = 0xC540;   // 3.7V

// voltage rail voltage range in mV
static volatile uint16_t vdd_upper_voltage;
static volatile uint16_t vdd_lower_voltage;

// measured voltage using ADC
static volatile uint16_t voltagemV;

/**************************************************************************//**
 * @brief  IADC Initializer
 *****************************************************************************/
void initIADC (IADC_PosInput_t posInput, IADC_NegInput_t negInput,
    uint16_t upperbound, uint16_t lowerbound)
{
  // Declare init structs
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

  // Reset IADC to reset configuration in case it has been modified
  IADC_reset(IADC0);

  // Configure IADC clock source to use the FSRCO(EM2/EM3)
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;

  // Set the clk_src_adc prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Set upper bound for window compare
  init.greaterThanEqualThres = upperbound;

  // Set lower bound for window compare
  init.lessThanEqualThres = lowerbound;

  // Configuration 0 is used by both scan and single conversions by default
  // Use internal 1.2V bandgap as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                              CLK_ADC_FREQ,
                                                                         0,
                                                         iadcCfgModeNormal,
                                                       init.srcClkPrescale);

  // Set conversions to run continuously
  initSingle.triggerAction = iadcTriggerActionContinuous;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = posInput;
  initSingleInput.negInput = negInput;

  // Enable window comparisons on this input
  initSingleInput.compare = true;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Scan
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Enable interrupts on window comparison match
  IADC_enableInt(IADC0, IADC_IF_SINGLECMP);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void IADC_IRQHandler(void)
{
  IADC_clearInt(IADC0, IADC_IF_SINGLECMP);

  // Read most recent sample
  sample = IADC_readSingleResult(IADC0);
  voltagemV = (sample.data)*4*1200/4095;

  // stop ADC conversion, set detected flag to true
  IADC_command(IADC0, iadcCmdStopSingle);
  detected_flag = true;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Init
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  // initialize global variables
  // VDD upper and lower voltage threshold in mV
  vdd_upper_voltage = (vdd_upper_bound>>4)*1200*4/4095;
  vdd_lower_voltage = (vdd_lower_bound>>4)*1200*4/4095;
  
  // Initialize IADC 
  if(AVDD_VMON){
    initIADC(iadcPosInputAvdd,iadcNegInputGnd,vdd_upper_bound,vdd_lower_bound);
  }
  else{
    initIADC(iadcPosInputDvdd,iadcNegInputGnd,vdd_upper_bound,vdd_lower_bound);
  }
  
  // Start scan
  IADC_command(IADC0, iadcCmdStartSingle);
  printf("Running AVDD/DVDD Voltage Monitoring\n");
  printf("-- Upper Voltage Bound: %dmV\n", vdd_upper_voltage);
  printf("-- Lower Voltage Bound: %dmV\n\n", vdd_lower_voltage);
  RETARGET_SerialFlush(); // delay for printf to finish

  while(1){
    // if compare mode triggered, update display,
    if(detected_flag){
      printf("\f");
      printf("AVDD/DVDD voltage out of range\n");
      printf("Expected Lower Voltage: %dmV\n", vdd_lower_voltage);
      printf("Expected Upper Voltage: %dmV\n", vdd_upper_voltage);
      printf("Voltage measured: %dmV\n\n", voltagemV);
      RETARGET_SerialFlush(); // delay for printf to finish
      detected_flag = false;
      IADC_command(IADC0, iadcCmdStartSingle);  // re-start scan
    }
    // Enter EM3, wait for next interrupt
    EMU_EnterEM3(true);
  }
}
