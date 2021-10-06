/***************************************************************************//**
 * @file main_scan_letimer_interrupt.c
 * @brief Use the IADC to take repeated nonblocking scan measurements on two
 * single-ended external inputs and six internal supplies, requested
 * periodically by LETIMER interrupt
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
#include "em_core.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_iadc.h"
#include "em_gpio.h"
#include "em_letimer.h"
#include "bsp.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Set CLK_ADC to 10MHz
#define CLK_SRC_ADC_FREQ          10000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ              10000000 // CLK_ADC - 10MHz max in normal mode

// Number of scan channels
#define NUM_INPUTS 8

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

// IADC scan complete GPIO toggle port/pin
#define IADC_OUTPUT_0_PORT        gpioPortD
#define IADC_OUTPUT_0_PIN         2

// Desired LETIMER frequency in Hz
#define LETIMER_FREQ              1

// LETIMER GPIO toggle port/pin (toggled in EM2; requires port A/B GPIO)
#define LETIMER_OUTPUT_0_PORT     gpioPortA
#define LETIMER_OUTPUT_0_PIN      5

/* This example enters EM2 in the infinite while loop; Setting this define to 1
 * enables debug connectivity in the EMU_CTRL register, which will consume about
 * 0.5uA additional supply current */
#define EM2DEBUG                  1

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   ******************************
 ******************************************************************************/

static volatile double scanResult[NUM_INPUTS];  // Volts

/**************************************************************************//**
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO (void)
{
  // Enable GPIO clock branch
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure IADC/LETIMER as outputs
  GPIO_PinModeSet(IADC_OUTPUT_0_PORT, IADC_OUTPUT_0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(LETIMER_OUTPUT_0_PORT, LETIMER_OUTPUT_0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief  IADC Initializer
 *****************************************************************************/
void initIADC (void)
{
  // Declare init structs
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;
  IADC_ScanTable_t initScanTable = IADC_SCANTABLE_DEFAULT;    // Scan Table

  // Enable IADC0 and GPIO clock branches
  CMU_ClockEnable(cmuClock_IADC0, true);

  // Reset IADC to reset configuration in case it has been modified by
  // other code
  IADC_reset(IADC0);

  // Select clock for IADC
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);  // FSRCO - 20MHz

  // Modify init structs and initialize
  init.warmup = iadcWarmupNormal;

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

  // Scan initialization
  initScan.dataValidLevel = _IADC_SCANFIFOCFG_DVL_VALID4;     // Set the SCANFIFODVL flag when 4 entries in the scan FIFO are valid;
                                                              // Not used as an interrupt or to wake up LDMA; flag is ignored

  // Tag FIFO entry with scan table entry id.
  initScan.showId = true;

  // Configure entries in scan table, CH0 is single-ended from input 0, CH1 is
  // single-ended from input 1
  initScanTable.entries[0].posInput = IADC_INPUT_0_PORT_PIN;
  initScanTable.entries[0].negInput = iadcNegInputGnd;
  initScanTable.entries[0].includeInScan = true;

  initScanTable.entries[1].posInput = IADC_INPUT_1_PORT_PIN;
  initScanTable.entries[1].negInput = iadcNegInputGnd;
  initScanTable.entries[1].includeInScan = true;

  initScanTable.entries[2].posInput = iadcPosInputAvdd;      // Add AVDD to scan for demonstration purposes
  initScanTable.entries[2].negInput = iadcNegInputGnd | 1;   // when measuring a supply, PINNEG needs to be odd (1, 3, 5,...)
  initScanTable.entries[2].includeInScan = true;

  initScanTable.entries[3].posInput = iadcPosInputVddio;     // Add VDDIO to scan for demonstration purposes
  initScanTable.entries[3].negInput = iadcNegInputGnd | 1;   // when measuring a supply, PINNEG needs to be odd (1, 3, 5,...)
  initScanTable.entries[3].includeInScan = true;

  initScanTable.entries[4].posInput = iadcPosInputVss;       // Add VSS to scan for demonstration purposes
  initScanTable.entries[4].negInput = iadcNegInputGnd | 1;   // when measuring a supply, PINNEG needs to be odd (1, 3, 5,...)
  initScanTable.entries[4].includeInScan = false;            // FIFO is only 4 entries deep

  initScanTable.entries[5].posInput = iadcPosInputVssaux;    // Add Vssaux to scan for demonstration purposes
  initScanTable.entries[5].negInput = iadcNegInputGnd | 1;   // when measuring a supply, PINNEG needs to be odd (1, 3, 5,...)
  initScanTable.entries[5].includeInScan = false;

  initScanTable.entries[6].posInput = iadcPosInputDvdd;      // Add DVDD to scan for demonstration purposes
  initScanTable.entries[6].negInput = iadcNegInputGnd | 1;   // when measuring a supply, PINNEG needs to be odd (1, 3, 5,...)
  initScanTable.entries[6].includeInScan = false;

  initScanTable.entries[7].posInput = iadcPosInputDecouple;  // Add Decouple to scan for demonstration purposes
  initScanTable.entries[7].negInput = iadcNegInputGnd | 1;   // when measuring a supply, PINNEG needs to be odd (1, 3, 5,...)
  initScanTable.entries[7].includeInScan = false;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Scan
  IADC_initScan(IADC0, &initScan, &initScanTable);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
  GPIO->IADC_INPUT_1_BUS |= IADC_INPUT_1_BUSALLOC;

  // Clear any previous interrupt flags
  IADC_clearInt(IADC0, _IADC_IF_MASK);

  // Enable Scan interrupts
  IADC_enableInt(IADC0, IADC_IEN_SCANTABLEDONE);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void IADC_IRQHandler(void)
{
  IADC_Result_t result = {0, 0};

  // Toggle GPIO
  GPIO_PinOutToggle(IADC_OUTPUT_0_PORT, IADC_OUTPUT_0_PIN);

  // Get ADC results
  while(IADC_getScanFifoCnt(IADC0))
  {
    // Read data from the scan FIFO
    result = IADC_pullScanFifoResult(IADC0);

    // Calculate input voltage:
    //  For single-ended the result range is 0 to +Vref, i.e.,
    //  for Vref = AVDD = 3.30V, 12 bits represents 3.30V full scale IADC range.
    scanResult[result.id] = result.data * 3.3 / 0xFFF;

    if((result.id > 1) && (result.id < 7))      // supply voltages other than vddlv are connected through a 1/4 voltage divider
    {
        scanResult[result.id] *= 4;             // rectify voltage divider to get actual voltage
    }
  }

  if(result.id == 3) // first set of table entries
  {
      IADC_setScanMask(IADC0, 0x00F0);  // configure scan mask to measure second set of table entries
  } else
  {
      IADC_setScanMask(IADC0, 0x000F);  // configure scan mask to measure first set of table entries
  }

  // Clear scan interrupt flag
  IADC_clearInt(IADC0, IADC_IF_SCANTABLEDONE); // flags are sticky; must be cleared in software
}

/**************************************************************************//**
 * @brief Clock initialization
 *****************************************************************************/
void initClock(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;

  // Select LFXO for the LETIMER
  CMU_LFXOInit(&lfxoInit);
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
}

/**************************************************************************//**
 * @brief LETIMER initialization
 *****************************************************************************/
void initLetimer(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Enable LETIMER0 clock tree
  CMU_ClockEnable(cmuClock_LETIMER0, true);

  // Calculate the top value (frequency) based on clock source
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / LETIMER_FREQ;

  // Reload top on underflow, toggle output, and run in free mode
  letimerInit.comp0Top = true;
  letimerInit.topValue = topValue;
  letimerInit.ufoa0 = letimerUFOAToggle;
  letimerInit.repMode = letimerRepeatFree;

  // Enable LETIMER0 output0
  GPIO->LETIMERROUTE.ROUTEEN = GPIO_LETIMER_ROUTEEN_OUT0PEN;
  GPIO->LETIMERROUTE.OUT0ROUTE = \
      (LETIMER_OUTPUT_0_PORT << _GPIO_LETIMER_OUT0ROUTE_PORT_SHIFT) \
      | (LETIMER_OUTPUT_0_PIN << _GPIO_LETIMER_OUT0ROUTE_PIN_SHIFT);

  // Initialize LETIMER
  LETIMER_Init(LETIMER0, &letimerInit);

  // Clear any previous interrupt flags
  LETIMER_IntClear(LETIMER0, _LETIMER_IF_MASK);

  // Enable underflow interrupts
  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_UF);

  // Enable LETIMER interrupts
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);
}

/**************************************************************************//**
 * @brief  LETIMER Handler
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
  uint32_t flags = LETIMER_IntGet(LETIMER0);

  // Trigger an IADC scan conversion
  IADC_command(IADC0, iadcCmdStartScan);

  // Clear LETIMER interrupt flags
  LETIMER_IntClear(LETIMER0, flags);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Initialize GPIO
  initGPIO();

  // Initialize the IADC
  initIADC();

  // Initialize LFXO
  initClock();

  // Initialize the LETIMER
  initLetimer();

#ifdef EM2DEBUG
  // Enable debug connectivity in EM2
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;
#endif

  while (1)
  {
    // Enter EM2 sleep
    EMU_EnterEM2(true);
  }
}
