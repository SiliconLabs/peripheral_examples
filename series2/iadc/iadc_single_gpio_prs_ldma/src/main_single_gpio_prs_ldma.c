/***************************************************************************//**
 * @brief Use the IADC to take repeated non-blocking measurements on single
 * input which in turn triggers the LDMA to transfer the IADC measurement to
 * memory, all while remaining in EM2. IADC conversion is triggered through GPIO
 * via PRS. After NUM_SAMPLES conversions the LDMA will trigger an interrupt
 * from EM2 and toggle LED0 on the WSTK.
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
#include "em_cmu.h"
#include "em_core.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_iadc.h"
#include "em_ldma.h"
#include "em_prs.h"

#include "bsp.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Set CLK_ADC to 10MHz
#define CLK_SRC_ADC_FREQ          20000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ              10000000 // CLK_ADC - 10 MHz max in normal mode

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

// Use specified LDMA/PRS channel
#define IADC_LDMA_CH              0
#define PRS_CHANNEL               0

// How many samples to capture
#define NUM_SAMPLES               10

/* This example enters EM2 in the main while() loop; Setting this #define to 1
 * enables debug connectivity in EM2, which increases current consumption by
 * about 0.5uA
 */
#define EM2DEBUG                  1

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

// LDMA link descriptor
LDMA_Descriptor_t descriptor;

// Buffer for IADC samples
uint32_t singleBuffer[NUM_SAMPLES];

/**************************************************************************//**
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO (void)
{
  // Enable GPIO clock branch
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LED0 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  // Configure pushbutton 0 as input and enable interrupt
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT,
                    BSP_GPIO_PB0_PIN,
                    BSP_GPIO_PB0_PIN,
                    false,
                    false,
                    false);
}

/**************************************************************************//**
 * @brief  PRS Initializer
 *****************************************************************************/
void initPRS (void)
{
  // Use GPIO as async PRS to trigger IADC in EM2
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Set up PRS GPIO and IADC as producer and consumer respectively */
  PRS_SourceAsyncSignalSet(PRS_CHANNEL, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO, BSP_GPIO_PB0_PIN);
  PRS_ConnectConsumer(PRS_CHANNEL, prsTypeAsync, prsConsumerIADC0_SINGLETRIGGER);
}

/**************************************************************************//**
 * @brief  IADC Initializer
 *****************************************************************************/
void initIADC (void)
{
  // Declare initialization structures
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

  CMU_ClockEnable(cmuClock_IADC0, true);

  // Use the FSRC0 as the IADC clock so it can run in EM2
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  /*
   * These two settings are modified from the defaults to reduce the
   * IADC current.  In low-frequency use cases, such as this example,
   * iadcWarmupNormal shuts down the IADC between conversions, which
   * reduces current at the expense of requiring 5 microseconds of
   * warm-up time before a conversion can begin.
   *
   * In cases where a PRS event triggers single conversions, enabling
   * iadcClkSuspend1 gates off the ADC_CLK until the PRS trigger event
   * occurs and again upon the completion of the channel converted.
   */
  init.warmup = iadcWarmupNormal;
  init.iadcClkSuspend1 = true;

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
   * conversion time = (4 * OSRHS) / fCLK_ADC
   *
   * ...which results in a maximum sampling rate of 1 Msps since
   * single-channel conversions do not incur the 2-clock input
   * multiplexer switching time associated with scan conversions.
   */
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                             CLK_ADC_FREQ,
                                             0,
                                             iadcCfgModeNormal,
                                             init.srcClkPrescale);

  /*
   * Trigger conversions on the PRS1 rising edge input (PRS1 is not a
   * specific channel but simply the dedicated trigger input for single
   * conversions.  PRS0 serves the same purpose for scan conversions.
   * Note that the IADC_TriggerSel_t typedef specifies PRS0 even when
   * PRS1 is used because the bit field encodings in the relevant
   * IADC_TRIGGER register are the same).
   *
   * Set the SINGLEFIFODVL flag when there is one entry in the output
   * FIFO.  Note that in this example, the interrupt associated with
   * the SINGLEFIFODVL flag in the IADC_IF register is not used.
   *
   * Enable DMA wake-up to save the results when the specified FIFO
   * level is hit.
   *
   * Allow a single conversion to start as soon as there is a trigger
   * event.
   */
  initSingle.triggerSelect = iadcTriggerSelPrs0PosEdge;
  initSingle.dataValidLevel = iadcFifoCfgDvl1;
  initSingle.fifoDmaWakeup = true;
  initSingle.start = true;

  initSingleInput.posInput   = IADC_INPUT_0_PORT_PIN;
  initSingleInput.negInput   = iadcNegInputGnd;

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Single
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);
}

/**************************************************************************//**
 * @brief
 *   LDMA Initializer
 *
 * @param[in] buffer
 *   pointer to the array where ADC data will be stored.
 *
 * @param[in] size
 *   size of the array
 *****************************************************************************/
void initLDMA(uint32_t *buffer, uint32_t size)
{
  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  // Configure LDMA for transfer from IADC to memory
  // LDMA will loop continuously
  LDMA_TransferCfg_t transferCfg =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_IADC0_IADC_SINGLE);

  // Set up descriptors for dual buffer transfer
  descriptor = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&IADC0->SINGLEFIFODATA, buffer, size, 0);

  // Initialize LDMA with default configuration
  LDMA_Init(&init);

  // Start transfer, LDMA will sample the IADC NUM_SAMPLES time, and then interrupt
  LDMA_StartTransfer(IADC_LDMA_CH, &transferCfg, &descriptor);
}

/**************************************************************************//**
 * @brief  LDMA interrupt handler
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

  // Initialize GPIO
  initGPIO();

  // Initialize PRS
  initPRS();

  // Initialize the IADC
  initIADC();

  // Initialize LDMA
  initLDMA(singleBuffer, NUM_SAMPLES);

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
