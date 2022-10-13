/***************************************************************************//**
 * @file main_single_20bit.c
 * @brief Uses the IADC repeated non-blocking measurements on a single input
 * while asleep and setting the oversampling field to achieve a 20-bit
 * resolution conversion. Sample clock is 10 MHz, and oversampling rate is 512.
 * This gives a sampling frequency of ~4.8 kHz. The IADC reads GPIO pin PA05
 * as input.
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
#include "em_prs.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Use specified PRS channel
#define IADC_PRS_CH              0

// Set CLK_ADC to 10MHz (this corresponds to a sample rate of 77K with OSR = 32)
// CLK_SRC_ADC; largest division is by 4
#define CLK_SRC_ADC_FREQ        20000000

// CLK_ADC; IADC_SCHEDx PRESCALE has 10 valid bits
#define CLK_ADC_FREQ            10000000

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
#define IADC_INPUT_0_PORT_PIN     iadcPosInputPortAPin5;

#define IADC_INPUT_0_BUS          ABUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_ABUSALLOC_AODD0_ADC0

// GPIO output toggle to notify IADC conversion complete
#define GPIO_OUTPUT_0_PORT        gpioPortB
#define GPIO_OUTPUT_0_PIN         1

/*
 * This example enters EM2 in the main while() loop; Setting this #define to 1
 * enables debug connectivity in EM2, which increases current consumption by
 * about 0.5uA
 */
#define EM2DEBUG                  1

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   ******************************
 ******************************************************************************/

// Stores latest ADC sample and converts to volts
static volatile IADC_Result_t sample;
static volatile double singleResult;

/**************************************************************************//**
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO clock branch

  /* Note: For EFR32xG21 radio devices, library function calls to 
   * CMU_ClockEnable() have no effect as oscillators are automatically turned
   * on/off based on demand from the peripherals; CMU_ClockEnable() is a dummy
   * function for EFR32xG21 for library consistency/compatibility.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);
   
  // Configure GPIO as output, will indicate when conversions are being performed
  GPIO_PinModeSet(GPIO_OUTPUT_0_PORT, GPIO_OUTPUT_0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief  PRS Initializer
 *****************************************************************************/
void initPRS(void)
{
  // Enable PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Connect PRS Async channel to ADC single complete signal
  PRS_SourceAsyncSignalSet(IADC_PRS_CH, PRS_ASYNC_CH_CTRL_SOURCESEL_IADC0,
                           PRS_ASYNC_CH_CTRL_SIGSEL_IADC0SINGLEDONE);

  // Route PRS channel to GPIO output to indicate a conversion complete
  PRS_PinOutput(IADC_PRS_CH, prsTypeAsync, GPIO_OUTPUT_0_PORT, GPIO_OUTPUT_0_PIN);
}

/**************************************************************************//**
 * @brief  IADC Initializer
 *****************************************************************************/
void initIADC(void)
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

  // Configure IADC clock source for use while in EM2
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Configuration 0 is used by both scan and single conversions by default
  // Use internal bandgap (supply voltage in mV) as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
  initAllConfigs.configs[0].vRef = 1210;
  initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain0P5x;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                    CLK_ADC_FREQ,
                                                                    0,
                                                                    iadcCfgModeNormal,
                                                                    init.srcClkPrescale);

  // Set oversampling rate to 32x; digital averaging to 16x
  // resolution formula res = 11 + log2(oversampling * digital averaging)
  // in this case res = 11 + log2(32 * 16) = 20
  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed32x;
  initAllConfigs.configs[0].digAvg = iadcDigitalAverage16;

  // Single initialization
  initSingle.dataValidLevel = iadcFifoCfgDvl1;

  // Set conversions to run continuously
  initSingle.triggerAction = iadcTriggerActionContinuous;

  // Set alignment to right justified with 20 bits for data field
  initSingle.alignment = iadcAlignRight20;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = IADC_INPUT_0_PORT_PIN;
  initSingleInput.negInput = iadcNegInputGnd;

  // Initialize IADC
  // Note oversampling and digital averaging will affect the offset correction
  // This is taken care of in the IADC_init() function in the emlib
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Scan
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;

  // Enable interrupts on data valid level
  IADC_enableInt(IADC0, IADC_IEN_SINGLEDONE);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
 * @brief  IADC interrupt handler
 *****************************************************************************/
void IADC_IRQHandler(void)
{
  // Read most recent single conversion result
  sample = IADC_readSingleResult(IADC0);

  // For single-ended the result range is 0 to +Vref, i.e., 20 bits for the
  // conversion value.
  singleResult = sample.data * 2.42 / 0xFFFFF;

  IADC_clearInt(IADC0, IADC_IF_SINGLEDONE);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Initialize GPIO
  initGPIO();

  // Initialize PRS
  initPRS();

  // Initialize the IADC
  initIADC();

#ifdef EM2DEBUG
#if (EM2DEBUG == 1)
  // Enable debug connectivity in EM2
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;
#endif
#endif

  // Start single
  IADC_command(IADC0, iadcCmdStartSingle);

  while (1)
  {
    // Enter EM2 sleep, woken by IADC interrupt
    EMU_EnterEM2(true);
  }
}
