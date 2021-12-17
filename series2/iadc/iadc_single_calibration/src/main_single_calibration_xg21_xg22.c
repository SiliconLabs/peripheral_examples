/***************************************************************************//**
 * @file main_single_calibration.c
 * @brief Use the IADC and an external supply to calibrate the gain and offset,
 * then take repeated blocking differential measurements on single set of
 * differential inputs
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_iadc.h"
#include "em_gpio.h"
#include "em_ldma.h"
#include "bsp.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// How many samples to capture and average
#define NUM_SAMPLES                 1024

// Set CLK_ADC to 10MHz
#define CLK_SRC_ADC_FREQ            20000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ                10000000 // CLK_ADC - 10MHz max in normal mode

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
#define IADC_INPUT_0_PORT_PIN     iadcPosInputPortCPin4;
#define IADC_INPUT_1_PORT_PIN     iadcNegInputPortCPin5;

#define IADC_INPUT_0_BUS          CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0
#define IADC_INPUT_1_BUS          CDBUSALLOC
#define IADC_INPUT_1_BUSALLOC     GPIO_CDBUSALLOC_CDODD0_ADC0

#define IADC_SCALE_OFFSET_MAX_NEG   0x00020000UL // 18-bit 2's compliment
#define IADC_SCALE_OFFSET_ZERO      0x00000000UL

// Push-buttons are active-low
#define PB_PRESSED (0)

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

static volatile IADC_Result_t sample;
static volatile double singleResult; // Volts

/**************************************************************************//**
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO (void)
{
  // Enable GPIO clock branch
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* Note: For EFR32xG21 radio devices, library function calls to 
   * CMU_ClockEnable() have no effect as oscillators are automatically turned
   * on/off based on demand from the peripherals; CMU_ClockEnable() is a dummy
   * function for EFR32xG21 for library consistency/compatibility.
   */
   
  // Configure push button PB0 as a user input; will use as a toggle to indicate when inputs are ready
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Configure LED0 as output, will indicate when conversions are being performed
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief  Initialize IADC function
 *****************************************************************************/
void initIADC (void)
{
  // Declare init structs
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

  // Enable IADC0 clock branch
  CMU_ClockEnable(cmuClock_IADC0, true);

  // Select clock for IADC
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);  // FSRCO - 20MHz

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Configuration 0 is used by both scan and single conversions by default
  // Use unbuffered AVDD as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
  initAllConfigs.configs[0].vRef = 3300;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                             CLK_ADC_FREQ,
                                             0,
                                             iadcCfgModeNormal,
                                             init.srcClkPrescale);

  initAllConfigs.configs[0].twosComplement = iadcCfgTwosCompBipolar; // Force IADC to use bipolar inputs for conversion

  // Assign pins to positive and negative inputs in differential mode
  initSingleInput.posInput   = IADC_INPUT_0_PORT_PIN;
  initSingleInput.negInput   = IADC_INPUT_1_PORT_PIN;

  // Initialize the IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize the Single conversion inputs
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
  GPIO->IADC_INPUT_1_BUS |= IADC_INPUT_1_BUSALLOC;
}

/**************************************************************************//**
 * @brief  Take several sequential samples and average the measurement
 *****************************************************************************/
double IADCAverageConversion(uint32_t numSamples)
{
    int i;
    double average;
    IADC_Result_t sample;

    // indicate to user IADC is busy
    GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

    // Averaging loop
    average = 0; // reset accumulator
    for(i = 0; i < numSamples; i++)
    {
      // Start IADC conversion
      IADC_command(IADC0, iadcCmdStartSingle);

      // Wait for conversion to be complete
      while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK
                  | _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV); //while combined status bits 8 & 6 don't equal 1 and 0 respectively

      // Get ADC result
      sample = IADC_pullSingleFifoResult(IADC0);
      average += (int32_t) sample.data;
    }
    average /= NUM_SAMPLES;

    // turn off indicator LED
    GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

    return average;
}

/**************************************************************************//**
 * @brief  IADC must be disabled to change scale
 *****************************************************************************/
void IADCRescale(uint32_t newScale)
{
    // Disable the IADC
    IADC0->EN_CLR = IADC_EN_EN;

    // configure new scale settings
    IADC0->CFG[0].SCALE = newScale;

    // Re-enable IADC
    IADC0->EN_SET = IADC_EN_EN;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  uint32_t scale;
  double calibration_gain13lsb;
  double result_fullscale, result_zero, result_offset;
  double gain_correction_factor;
  double IADC_GAIN13LSB_LSB = 0.25 / 8192; // LSB Gain multiplication factor
  uint32_t IADC_CALIBRATED_GAIN13LSB;
  int32_t IADC_CALIBRATED_OFFSET;

  CHIP_Init();

  initGPIO();

  initIADC();

  // Set initial offset to maximum negative and initial gain to 1.0
  scale = IADC_SCALE_GAIN3MSB_GAIN100 | IADC_SCALE_GAIN13LSB_DEFAULT | IADC_SCALE_OFFSET_MAX_NEG;

  IADCRescale(scale);

  // Apply a full-scale positive input to the IADC
  // Wait until differential voltage is applied
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) != PB_PRESSED); //user feedback
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == PB_PRESSED); //make it a toggle

  // Take multiple conversions and average to reduce system-level noise
  result_fullscale = IADCAverageConversion(NUM_SAMPLES);

  // Apply a zero differential input to the IADC (short P25-P27)
  // Wait until differential voltage is applied
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) != PB_PRESSED); // user feedback
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == PB_PRESSED);

  result_zero = IADCAverageConversion(NUM_SAMPLES);

  // Calculate gain correction factor
  //  In bipolar mode, expected positive full-scale for IADC is (2^11) - 1 = 2047
  gain_correction_factor = 2047 / (result_fullscale - result_zero);

  // Set IADC correction gain and clear offset in order to calibrate offset
  //  3 MSB of gain represented by 1 bit; 1 => 100 representing 1.00x to 1.2499x, 0 => 011 representing 0.75x to .9999x
  if(gain_correction_factor >= 1.0)
  {
      calibration_gain13lsb = (gain_correction_factor - 1.0) / IADC_GAIN13LSB_LSB;
      IADC_CALIBRATED_GAIN13LSB = (uint32_t) (calibration_gain13lsb + 0.5); // round to the nearest integer
      scale = IADC_SCALE_GAIN3MSB_GAIN100 | (IADC_CALIBRATED_GAIN13LSB << _IADC_SCALE_GAIN13LSB_SHIFT) | IADC_SCALE_OFFSET_ZERO;
  } 
  else
  {
      calibration_gain13lsb = (gain_correction_factor - 0.75) / IADC_GAIN13LSB_LSB;
      IADC_CALIBRATED_GAIN13LSB = (uint32_t) (calibration_gain13lsb + 0.5); // round to the nearest integer
      scale = IADC_SCALE_GAIN3MSB_GAIN011 | (IADC_CALIBRATED_GAIN13LSB << _IADC_SCALE_GAIN13LSB_SHIFT) | IADC_SCALE_OFFSET_ZERO;
  }

  IADCRescale(scale);

  result_offset = IADCAverageConversion(NUM_SAMPLES);

  // scale and negate offset
  //  OFFSET is encoded as a 2's complement, 18-bit number with the LSB representing 1 / (2^20) of full scale.
  IADC_CALIBRATED_OFFSET = (int32_t) (result_offset * -256);

  // 18-bit boundary check [-2^17 <-> (2^17-1)]
  //  Check if offset is too large to be corrected and set to maximum correction allowed
  if (IADC_CALIBRATED_OFFSET > 131071)
      IADC_CALIBRATED_OFFSET = 131071;
  if (IADC_CALIBRATED_OFFSET < -131072)
      IADC_CALIBRATED_OFFSET = -131072;

  // Maintain previous gain correction; apply offset correction
  if(gain_correction_factor >= 1.0)
  {
      scale = IADC_SCALE_GAIN3MSB_GAIN100 | (IADC_CALIBRATED_GAIN13LSB << _IADC_SCALE_GAIN13LSB_SHIFT)
              | (IADC_CALIBRATED_OFFSET & _IADC_SCALE_OFFSET_MASK);
  }
  else
  {
      scale = IADC_SCALE_GAIN3MSB_GAIN011 | (IADC_CALIBRATED_GAIN13LSB << _IADC_SCALE_GAIN13LSB_SHIFT)
              | (IADC_CALIBRATED_OFFSET & _IADC_SCALE_OFFSET_MASK);
  }

  IADCRescale(scale);

  // Infinite loop
  while(1)
  {
    // Start IADC conversion
    IADC_command(IADC0, iadcCmdStartSingle);

    // Wait for conversion to be complete
    while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK
                | _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV); //while combined status bits 8 & 6 don't equal 1 and 0 respectively

    // Get ADC result
    sample = IADC_pullSingleFifoResult(IADC0);

    // Calculate input voltage:
    //  For differential inputs, the resultant range is from -Vref to +Vref, i.e.,
    //  for Vref = AVDD = 3.30V, 12 bits represents 6.60V full scale IADC range.
    singleResult = ((int32_t) sample.data * 6.6) / 0xFFF;
  }
}
