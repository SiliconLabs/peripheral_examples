/***************************************************************************//**
 * @file main_scan_iadc_timer.c
 * @brief Use the IADC to take repeated, non-blocking measurements on
 * multiple inputs using IADC local timer to trigger conversions.
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
#include "em_gpio.h"
#include "em_iadc.h"
#include "bsp.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Set CLK_ADC to 40 MHz - this will be adjusted to HFXO frequency in the
// initialization process
#define CLK_SRC_ADC_FREQ        40000000  // CLK_SRC_ADC - 40 MHz max
#define CLK_ADC_FREQ            10000000  // CLK_ADC - 10 MHz max in normal mode

// Number of scan channels
#define NUM_INPUTS 2

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
#define IADC_INPUT_0_PORT_PIN     iadcPosInputPortBPin0;
#define IADC_INPUT_1_PORT_PIN     iadcPosInputPortBPin1;

#define IADC_INPUT_0_BUS          BBUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_BBUSALLOC_BEVEN0_ADC0
#define IADC_INPUT_1_BUS          BBUSALLOC
#define IADC_INPUT_1_BUSALLOC     GPIO_BBUSALLOC_BODD0_ADC0

// GPIO output toggle to notify IADC conversion complete
#define GPIO_OUTPUT_0_PORT        gpioPortC
#define GPIO_OUTPUT_0_PIN         5

#if defined(BSP_WSTK_BRD4181A) || defined(BSP_WSTK_BRD4182A)
// HFXO frequency set for BRD4181A and BRD4182A
#define HFXO_FREQ               38400000
#else
// HFXO frequency for rest of radio boards
#define HFXO_FREQ               39000000
#endif

/*******************************************************************************
***************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

static volatile double scanResult[NUM_INPUTS];

/**************************************************************************//**
 * @brief  CMU initialization
 *****************************************************************************/
void initCMU(void)
{
  // Initialization structure for HFXO configuration
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Check if device has HFXO configuration information in DEVINFO page
  if (DEVINFO->MODULEINFO & DEVINFO_MODULEINFO_HFXOCALVAL) {
    // Use the DEVINFO page's CTUNE values
    hfxoInit.ctuneXoAna = (DEVINFO->MODXOCAL & DEVINFO_MODXOCAL_HFXOCTUNEXOANA_DEFAULT)
        >> _DEVINFO_MODXOCAL_HFXOCTUNEXOANA_SHIFT;
    hfxoInit.ctuneXiAna = (DEVINFO->MODXOCAL & DEVINFO_MODXOCAL_HFXOCTUNEXIANA_DEFAULT)
        >> _DEVINFO_MODXOCAL_HFXOCTUNEXIANA_SHIFT;
  }

  // Configure HFXO settings
  CMU_HFXOInit(&hfxoInit);

  // Set system HFXO frequency
  SystemHFXOClockSet(HFXO_FREQ);

  // Enable HFXO oscillator, and wait for it to be stable
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  // Select HFXO as the EM01GRPA clock
  CMU_ClockSelectSet(cmuClock_EM01GRPACLK, cmuSelect_HFXO);
}

/**************************************************************************//**
 * @brief  GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  /*
   * On EFR32xG21 devices, CMU_ClockEnable() calls have no effect as
   * clocks are enabled/disabled on-demand in response to peripheral
   * requests.  Deleting such lines is safe on xG21 devices and will
   * provide a small reduction in code size.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);
   
  // Output toggled upon completing each IADC scan sequence
  GPIO_PinModeSet(GPIO_OUTPUT_0_PORT, GPIO_OUTPUT_0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief  IADC initialization
 *****************************************************************************/
void initIADC(void)
{
  // Declare initialization structures
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;

  // Scan table structure
  IADC_ScanTable_t scanTable = IADC_SCANTABLE_DEFAULT;

  CMU_ClockEnable(cmuClock_IADC0, true);

  // Use the EM01GRPACLK as the IADC clock
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_EM01GRPACLK);

  // Shutdown between conversions to reduce current
  init.warmup = iadcWarmupNormal;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  /*
   * The IADC local timer runs at CLK_SRC_ADC_FREQ, which is at least
   * 2x CLK_ADC_FREQ. CLK_SRC_ADC_FREQ in this example is equal to the
   * HFXO frequency. Dividing the frequency of the HFXO by 1000 will give
   * the tick count for 1 ms trigger rate.
   * For example - if HFXO freq = 38.4 MHz,
   *
   * ticks for 1 ms trigger = 38400000 / 1000
   * ticks =  38400
   */
  init.timerCycles = CMU_ClockFreqGet(cmuClock_IADCCLK)/1000;

  /*
   * Configuration 0 is used by both scan and single conversions by
   * default.  Use internal bandgap as the reference and specify the
   * reference voltage in mV.
   *
   * Resolution is not configurable directly but is based on the
   * selected oversampling ratio (osrHighSpeed), which defaults to
   * 2x and generates 12-bit results.
   */
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
  initAllConfigs.configs[0].vRef = 1210;
  initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;
  initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain0P5x;
  
  /*
   * CLK_SRC_ADC must be prescaled by some value greater than 1 to
   * derive the intended CLK_ADC frequency.
   *
   * Based on the default 2x oversampling rate (OSRHS)...
   *
   * conversion time = ((4 * OSRHS) + 2) / fCLK_ADC
   *
   * ...which results in a maximum sampling rate of 833 ksps with the
   * 2-clock input multiplexer switching time is included.
   */
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                     CLK_ADC_FREQ,
                                                                     0,
                                                                     iadcCfgModeNormal,
                                                                     init.srcClkPrescale);

  /*
   * The IADC local timer triggers conversions.
   *
   * Set the SCANFIFODVL flag when scan FIFO holds 2 entries.  In this
   * example, the interrupt associated with the SCANFIFODVL flag in
   * the IADC_IF register is not used.
   *
   * Tag each FIFO entry with scan table entry ID.
   */
  initScan.triggerSelect = iadcTriggerSelTimer;
  initScan.dataValidLevel = iadcFifoCfgDvl2;
  initScan.showId = true;

  /*
   * Configure entries in the scan table.  CH0 is single-ended from
   * input 0; CH1 is single-ended from input 1.
   */
  scanTable.entries[0].posInput = IADC_INPUT_0_PORT_PIN;
  scanTable.entries[0].negInput = iadcNegInputGnd;
  scanTable.entries[0].includeInScan = true;

  scanTable.entries[1].posInput = IADC_INPUT_1_PORT_PIN;
  scanTable.entries[1].negInput = iadcNegInputGnd;
  scanTable.entries[1].includeInScan = true;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize scan
  IADC_initScan(IADC0, &initScan, &scanTable);

  // Enable the IADC timer (must be after the IADC is initialized)
  IADC_command(IADC0, iadcCmdEnableTimer);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
  GPIO->IADC_INPUT_1_BUS |= IADC_INPUT_1_BUSALLOC;
  
  // Enable scan interrupts
  IADC_enableInt(IADC0, IADC_IEN_SCANFIFODVL);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
 * @brief  IADC interrupt handler
 *****************************************************************************/
void IADC_IRQHandler(void)
{
  IADC_Result_t sample;

  // Toggle GPIO to signal scan completion
  GPIO_PinOutToggle(GPIO_OUTPUT_0_PORT, GPIO_OUTPUT_0_PIN);

  // While the FIFO count is non-zero...
  while (IADC_getScanFifoCnt(IADC0))
  {
    // Pull a scan result from the FIFO
    sample = IADC_pullScanFifoResult(IADC0);

    /*
     * Calculate the voltage converted as follows:
     *
     * For single-ended conversions, the result can range from 0 to
     * +Vref, i.e., for Vref = VBGR = 1.21V, and with analog gain = 0.5,
     * 0xFFF represents the full scale value of 2.42V.
     */
    scanResult[sample.id] = sample.data * 2.42 / 0xFFF;
  }

  /*
   * Clear the scan table complete interrupt.  Reading from the FIFO
   * does not do this automatically.
   */
  IADC_clearInt(IADC0, IADC_IF_SCANFIFODVL);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  initCMU();

  initGPIO();

  initIADC();

  // Start scan
  IADC_command(IADC0, iadcCmdStartScan);

  // Infinite loop
  while (1);
}
