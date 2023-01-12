/***************************************************************************//**
 * @file main_scan_letimer_prs_ldma.c
 *
 * @brief Use the IADC to take repeated, non-blocking measurements on
 * two external inputs.  The LETIMER triggers conversions via the PRS,
 * and the LDMA transfers the results to RAM, all while remaining in
 * EM2.
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

#include "em_device.h"
#include "em_chip.h"
#include "em_core.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_iadc.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_ldma.h"
#include "em_letimer.h"

#include "bspconfig.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Set CLK_ADC to 10MHz
#define CLK_SRC_ADC_FREQ    20000000  // CLK_SRC_ADC
#define CLK_ADC_FREQ        10000000  // CLK_ADC - 10 MHz max in normal mode

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
#define IADC_INPUT_0_PORT_PIN     iadcPosInputPortAPin0;
#define IADC_INPUT_1_PORT_PIN     iadcPosInputPortAPin5;

#define IADC_INPUT_0_BUS          ABUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_ABUSALLOC_AEVEN0_ADC0
#define IADC_INPUT_1_BUS          ABUSALLOC
#define IADC_INPUT_1_BUSALLOC     GPIO_ABUSALLOC_AODD0_ADC0

// Desired LETIMER frequency in Hz
#define LETIMER_FREQ              1

// LETIMER GPIO toggle port/pin (toggled in EM2; requires port A/B GPIO)
#define LETIMER_OUTPUT_0_PORT     gpioPortA
#define LETIMER_OUTPUT_0_PIN      6

// Use specified LDMA/PRS channel
#define IADC_LDMA_CH              0
#define PRS_CHANNEL               0

// How many samples to take
#define NUM_SAMPLES               10

/*
 * This example enters EM2 in the main while() loop; Setting this #define
 * to 1 enables debug connectivity in EM2, which increases current
 * consumption by about 0.5 uA.
 */
#define EM2DEBUG                  1

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

/// Globally declared LDMA link descriptor
LDMA_Descriptor_t descriptor;

// buffer to store IADC samples
uint32_t scanBuffer[NUM_SAMPLES];

/**************************************************************************//**
 * @brief  GPIO initialization
 *****************************************************************************/
void initGPIO (void)
{
  /*
   * Note: On EFR32xG21 devices, CMU_ClockEnable() calls have no effect
   * as clocks are enabled/disabled on-demand in response to peripheral
   * requests.  Deleting such lines is safe on xG21 devices and will
   * provide a small reduction in code size.
   */
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Show sample completion state on LED0
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  // Show LETIMER activity
  GPIO_PinModeSet(LETIMER_OUTPUT_0_PORT, LETIMER_OUTPUT_0_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief  PRS initialization
 *****************************************************************************/
void initPRS (void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  // Connect the specified PRS channel to the LETIMER producer
  PRS_SourceAsyncSignalSet(PRS_CHANNEL,
                           PRS_ASYNC_CH_CTRL_SOURCESEL_LETIMER0,
                           PRS_LETIMER0_CH0);

  // Connect the specified PRS channel to the IADC as the consumer
  PRS_ConnectConsumer(PRS_CHANNEL,
                      prsTypeAsync,
                      prsConsumerIADC0_SCANTRIGGER);
}

/**************************************************************************//**
 * @brief  IADC initialization
 *****************************************************************************/
void initIADC (void)
{
  // Declare initialization structures
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;

  // Scan table structure
  IADC_ScanTable_t scanTable = IADC_SCANTABLE_DEFAULT;

  CMU_ClockEnable(cmuClock_IADC0, true);

  // Use the FSRC0 as the IADC clock so it can run in EM2
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  // Set the prescaler needed for the intended IADC clock frequency
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  /*
   * These two settings are modified from the defaults to reduce the
   * IADC current.  In low-frequency use cases, such as this example,
   * iadcWarmupNormal shuts down the IADC between conversions, which
   * reduces current at the expense of requiring 5 microseconds of
   * warm-up time before a conversion can begin.
   *
   * In cases where a PRS event triggers scan conversions, enabling
   * iadcClkSuspend0 gates off the ADC_CLK until the PRS trigger event
   * occurs and again upon the completion of the channel conversions
   * specified in the scan table.
   */
  init.warmup = iadcWarmupNormal;
  init.iadcClkSuspend0 = true;

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
   * CLK_SRC_ADC is prescaled to derive the intended CLK_ADC frequency.
   *
   * Based on the default 2x oversampling rate (OSRHS)...
   *
   * conversion time = ((4 * OSRHS) + 2) / fCLK_ADC
   *
   * ...which, results in a maximum sampling rate of 833 ksps with the
   * 2-clock input multiplexer switching time is included.
   */
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                     CLK_ADC_FREQ,
                                                                     0,
                                                                     iadcCfgModeNormal,
                                                                     init.srcClkPrescale);

  /*
   * Trigger conversions on the PRS0 rising edge input (PRS0 is not a
   * specific channel but simply the dedicated trigger input for scan
   * conversions; PRS1 serves the same purpose for single conversions).
   *
   * Set the SCANFIFODVL flag when there are 2 entries in the scan
   * FIFO.  Note that in this example, the interrupt associated with
   * the SCANFIFODVL flag in the IADC_IF register is not used.
   *
   * Enable DMA wake-up to save the results when the specified FIFO
   * level is hit.
   *
   * Allow a scan conversion sequence to start as soon as there is a
   * trigger event.
   */
  initScan.triggerSelect = iadcTriggerSelPrs0PosEdge;
  initScan.dataValidLevel = _IADC_SCANFIFOCFG_DVL_VALID2;
  initScan.fifoDmaWakeup = true;
  initScan.start = true;

  /*
   * Configure entries in scan table.  CH0 is single-ended from
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

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
  GPIO->IADC_INPUT_1_BUS |= IADC_INPUT_1_BUSALLOC;
}

/**************************************************************************//**
 * @brief LETIMER initialization
 *****************************************************************************/
void initLETIMER(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Initialize the LFXO and use it as the EM23GRPACLK source
  CMU_LFXOInit(&lfxoInit);
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);

  CMU_ClockEnable(cmuClock_LETIMER0, true);

  // Calculate the top value (frequency) based on clock source
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / LETIMER_FREQ;

  // Reload top on underflow, pulse output, and run in free mode
  letimerInit.comp0Top = true;
  letimerInit.topValue = topValue;
  letimerInit.ufoa0 = letimerUFOAPulse;
  letimerInit.repMode = letimerRepeatFree;

  // Enable LETIMER0 output0
  GPIO->LETIMERROUTE[0].ROUTEEN = GPIO_LETIMER_ROUTEEN_OUT0PEN;
  GPIO->LETIMERROUTE[0].OUT0ROUTE = (LETIMER_OUTPUT_0_PORT << _GPIO_LETIMER_OUT0ROUTE_PORT_SHIFT) |
                                    (LETIMER_OUTPUT_0_PIN << _GPIO_LETIMER_OUT0ROUTE_PIN_SHIFT);

  // Initialize LETIMER
  LETIMER_Init(LETIMER0, &letimerInit);
}

/**************************************************************************//**
 * @brief
 *   LDMA initialization
 *
 * @param[in] buffer
 *   pointer to the array where ADC results will be stored.
 * @param[in] size
 *   size of the array
 *****************************************************************************/
void initLDMA(uint32_t *buffer, uint32_t size)
{
  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  // Trigger LDMA transfer on IADC scan completion
  LDMA_TransferCfg_t transferCfg =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_IADC0_IADC_SCAN);

  /*
   * Set up a linked descriptor to save scan results to the
   * user-specified buffer.  By linking the descriptor to itself
   * (the last argument is the relative jump in terms of the number of
   * descriptors), transfers will run continuously until firmware
   * otherwise stops them.
   */
  descriptor =
    (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&(IADC0->SCANFIFODATA), buffer, size, 0);

  // Initialize LDMA with default configuration
  LDMA_Init(&init);

  /*
   * Start the LDMA channel.  The first transfer will not occurs until
   * the LETIMER counts down to 0 and generates a pulse that is
   * routed to the IADC scan trigger input via the PRS. 
   */
  LDMA_StartTransfer(IADC_LDMA_CH, &transferCfg, &descriptor);
}

/**************************************************************************//**
 * @brief  LDMA Handler
 *****************************************************************************/
void LDMA_IRQHandler(void)
{
  // Clear interrupt flags
  LDMA_IntClear(LDMA_IF_DONE0);

  // Toggle LED0 to notify that transfers are complete
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  initGPIO();

  initPRS();

  initIADC();

  // Initialize LDMA
  initLDMA(scanBuffer, NUM_SAMPLES);

  // Initialize the LETIMER (starts conversions)
  initLETIMER();

#ifdef EM2DEBUG
#if (EM2DEBUG == 1)
  // Enable debug connectivity in EM2
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;
#endif
#endif

  while (1)
  {
    // Enter EM2 sleep
    EMU_EnterEM2(true);
  }
}
