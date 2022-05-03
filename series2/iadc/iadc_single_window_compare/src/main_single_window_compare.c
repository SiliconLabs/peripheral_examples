/***************************************************************************//**
 * @brief Uses the IADC as a window comparator on a single pin. Input is taken
 * on PC05 and PB01 (WSTK LED0) toggles on each comparator trigger. The most
 * recent sample within the window comparison is also stored globally.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_iadc.h"
#include "em_gpio.h"
#include "bsp.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Set CLK_ADC to 100 kHz (this corresponds to a sample rate of 10 ksps)
#define CLK_SRC_ADC_FREQ        9600000  // CLK_SRC_ADC
#define CLK_ADC_FREQ            100000   // CLK_ADC

// Upper and lower bound for Window Comparator
// 16-bit left-justified format; 12-bit conversion result compared to upper 12-bits of window comparator
#define WINDOW_UPPER_BOUND      0xC000   // 1.815V
#define WINDOW_LOWER_BOUND      0x4000   // 0.605V

/*
 * Specify the IADC input using the IADC_PosInput_t typedef.  This
 * must be paired with a corresponding macro definition that allocates
 * the corresponding ABUS to the IADC.  These are...
 *
 * GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN0_ADC0
 * GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AODD0_ADC0
 * GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BEVEN0_ADC0
 * GPIO->BBUSALLOC |= GPIO_BBUSALLOC_BODD0_ADC0
 * GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0
 * GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDODD0_ADC0
 *
 * ...for port A, port B, and port C/D pins, even and odd, respectively.
 */
#define IADC_INPUT_0_PORT_PIN     iadcPosInputPortCPin5;

#define IADC_INPUT_0_BUS          CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_CDBUSALLOC_CDODD0_ADC0

// HFXO frequency set for most common radio configurations
#define HFXO_FREQ               38400000

// CTUNE value for the radio board HFXO oscillator
#define HFXO_CTUNE_VALUE        BSP_HFXO_CTUNE

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

/// Stores latest ADC sample
static volatile IADC_Result_t sample;
static volatile double singleResult;

/**************************************************************************//**
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO (void)
{
  // Enable GPIO clock branch

  /* Note: For EFR32xG21 radio devices, library function calls to 
   * CMU_ClockEnable() have no effect as oscillators are automatically turned
   * on/off based on demand from the peripherals; CMU_ClockEnable() is a dummy
   * function for EFR32xG21 for library consistency/compatibility.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);
   
  // Configure LED0 as output; enable WSTK LED0
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief  IADC Initializer
 *****************************************************************************/
void initIADC (void)
{
  // Declare init structs
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

  // Enable IADC clock
  CMU_ClockEnable(cmuClock_IADC0, true);

  // Reset IADC to reset configuration in case it has been modified
  IADC_reset(IADC0);

  // Select HFXO as the EM01GRPA clock
  CMU_ClockSelectSet(cmuClock_EM01GRPACLK, cmuSelect_HFXO);

  // Configure IADC clock source to use the HFXO (EM01GRPA)
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_EM01GRPACLK);

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Set lower bound for window compare
  init.greaterThanEqualThres = WINDOW_LOWER_BOUND;

  // Set upper bound for window compare
  init.lessThanEqualThres = WINDOW_UPPER_BOUND;

  // Configuration 0 is used by both scan and single conversions by default
  // Use internal bandgap (supply voltage in mV) as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
  initAllConfigs.configs[0].vRef = 1210;
  initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain0P5x;

  // Divide CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                    CLK_ADC_FREQ,
                                                                    0,
                                                                    iadcCfgModeNormal,
                                                                    init.srcClkPrescale);

  // Set conversions to run continuously
  initSingle.triggerAction = iadcTriggerActionContinuous;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = IADC_INPUT_0_PORT_PIN;
  initSingleInput.negInput = iadcNegInputGnd;

  // Enable window comparisons on this input
  initSingleInput.compare = true;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Scan
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;

  // Enable interrupts on window comparison match
  IADC_enableInt(IADC0, IADC_IEN_SINGLECMP);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

void initHFXO(void)
{
  // Initialization structure for HFXO configuration
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Apply board specific tuning capacitor values
  hfxoInit.ctuneXiAna = HFXO_CTUNE_VALUE;
  hfxoInit.ctuneXoAna = HFXO_CTUNE_VALUE;

  // Configure HFXO settings
  CMU_HFXOInit(&hfxoInit);

  // Set system HFXO frequency
  SystemHFXOClockSet(HFXO_FREQ);

  // Enable HFXO oscillator, and wait for it to be stable
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
}

/**************************************************************************//**
 * @brief  IADC interrupt handler
 *****************************************************************************/
void IADC_IRQHandler(void)
{
  IADC_clearInt(IADC0, IADC_IF_SINGLECMP);

  // Read most recent sample
  sample = IADC_readSingleResult(IADC0);

  // Calculate input voltage:
  // For single-ended the result range is 0 to +Vref, i.e.,
  // for Vref = VBGR = 1.21V, and with analog gain = 0.5,
  // 12 bits represents 2.42V full scale IADC range.
  singleResult = sample.data * 2.42 / 0xFFF;

  // Toggle WSTK LED0 to signal compare event
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Initialize GPIO
  initGPIO();

  // Startup and select HFXO as clock source for system and EM01GRPA
  initHFXO();

  // Initialize the IADC
  initIADC();

  // Start scan
  IADC_command(IADC0, iadcCmdStartSingle);

  // Infinite loop
  while(1);
}
