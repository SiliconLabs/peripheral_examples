/***************************************************************************//**
 * @file main_single_low_current_xG21.c
 * @brief Uses the internal IADC timer to trigger a single-ended conversion,
 * which in turn triggers the LDMA to transfer the IADC measurement to memory,
 * all while remaining in EM2. After NUM_SAMPLES conversions the LDMA will
 * trigger an interrupt from EM2 and toggle LED0 on the WSTK.
 * This example is meant to be profiled using Energy Profiler to observe lower
 * current draw in EM2.
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rtcc.h"
#include "em_iadc.h"
#include "em_ldma.h"
#include "em_gpio.h"
#include "bsp.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Use specified LDMA channel
#define IADC_LDMA_CH              0

// How many samples to capture
#define NUM_SAMPLES               1024

// Set HFRCOEM23 to lowest frequency (1 MHz)
#define HFRCOEM23_FREQ            cmuHFRCOEM23Freq_1M0Hz

// Set CLK_ADC to 1 MHz (set to max for shortest IADC conversion/power-up time)
#define CLK_SRC_ADC_FREQ          1000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ              1000000 // CLK_ADC

// Set IADC timer cycles
#define TIMER_CYCLES              10000  // 10000 => 100   samples/second
                                         // 1000  => 1000  samples/second
                                         // 200   => 5000  samples/second
                                         // 100   => 10000 samples/second
                                         // 40    => 25000 samples/second

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

// Push-buttons are active-low
#define PB_PRESSED (0)

/*
 * This example enters EM2 in the infinite while loop; Setting this define to 1
 * enables debug connectivity in the EMU_CTRL register, which will consume about
 * 0.5uA additional supply current; defaults off for Energy Profiler
 */
#define EM2DEBUG                  0

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
  // Configure push button 0 pin as an input; wait for toggle to begin
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Configure LED0 as output; toggles when LDMA completes transfers
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

  // Reset IADC to reset configuration in case it has been modified
  IADC_reset(IADC0);

  // Configure IADC clock source for use while in EM2
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_HFRCOEM23); // 1MHz

  // Modify init structs and initialize
  init.warmup = iadcWarmupNormal;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Set timer cycles to configure sampling rate
  init.timerCycles = TIMER_CYCLES;

  // Configuration 0 is used by both scan and single conversions by default
  // Use internal bandgap (supply voltage in mV) as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;
  initAllConfigs.configs[0].vRef = 1210;
  initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain0P5x;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency
  // Default oversampling (OSR) is 2x, and Conversion Time = ((4 * OSR) + 2) / fCLK_ADC
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                    CLK_ADC_FREQ,
                                                                    0,
                                                                    iadcCfgModeNormal,
                                                                    init.srcClkPrescale);

  // Single initialization
  initSingle.triggerSelect = iadcTriggerSelTimer;
  initSingle.dataValidLevel = iadcFifoCfgDvl1;

  // Enable triggering of single conversion
  initSingle.start = true;

  // Set to run in EM2
  initSingle.fifoDmaWakeup = true;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = IADC_INPUT_0_PORT_PIN;
  initSingleInput.negInput = iadcNegInputGnd;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Allocate the corresponding ABUS to the IADC
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;

  // Initialize Single
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);
}

/**************************************************************************//**
 * @brief
 *   LDMA Initializer
 *
 * @param[in] buffer
 *   pointer to the array where ADC data will be stored.
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

  // Loop of NUM_SAMPLES, run continuously
  descriptor.xfer.decLoopCnt = 0;
  descriptor.xfer.xferCnt = NUM_SAMPLES;

  // Interrupt upon transfer complete
  descriptor.xfer.doneIfs = 1;
  descriptor.xfer.ignoreSrec = 0;

  // Initialize LDMA with default configuration
  LDMA_Init(&init);

  // Start transfer, LDMA will sample the IADC NUM_SAMPLES time, and then interrupt
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
 * @brief
 *   RTCC Initializer
 *
 * @param[in] osc
 *   Oscillator to run RTCC from (LFXO or LFRCO).
 *****************************************************************************/
void initRTCC(CMU_Select_TypeDef osc)
{
  // Route desired oscillator to RTCC clock tree.
  CMU_ClockSelectSet(cmuClock_RTCCCLK, osc);

  // Setup RTC parameters
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_256;
  rtccInit.cntWrapOnCCV1 = true;

  // Initialize RTCC. Configure RTCC with prescaler 256.
  RTCC_Init(&rtccInit);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;

  CHIP_Init();

  // Use default settings for EM23
  EMU_EM23Init(&em23Init);

  // Select FSRCO for SYSCLK; FSRCO is default out of reset
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);

  // Initialize GPIO
  initGPIO();

  /*
   * Debugging trap; wait here for PB0 toggle before entering EM2.
   * Allows EM0 current to be observed in Energy Profiler.
   */
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) != PB_PRESSED);
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == PB_PRESSED);

  // Turn off push button 0 GPIO input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 0);

#ifdef EM2DEBUG
#if (EM2DEBUG == 1)
  // Enable debug connectivity in EM2
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;
#endif
#endif

  /* Initialize RTCC; Attempting to match datasheet current consumption test
   * condition - RTCC running from LFRCO; full RAM retention
   */
  initRTCC(cmuSelect_LFRCO);

  // Set HFRCOEM23 (used by IADC) to specified frequency band
  CMU_HFRCOEM23BandSet(HFRCOEM23_FREQ);

  // Initialize the IADC
  initIADC();

  // Initialize LDMA
  initLDMA(singleBuffer, NUM_SAMPLES);

  // IADC single already enabled; must enable timer block in order to trigger
  IADC_command(IADC0, iadcCmdEnableTimer);

  /* Infinite loop; once LDMA transfer completes, wake up, handle IRQ, and
   * return to EM2 until next LDMA transfer completes
   */
  while (1)
  {
    // Enter EM2.
    EMU_EnterEM2(true);
  }
}
