/***************************************************************************//**
 * @brief Uses the IADC and LDMA to take repeated nonblocking measurements on
 * multiple inputs while asleep without any processor overhead. Sample rate is
 * 833 ksps, and the ADC reads GPIO pins PC4 (P25 on BRD4001 J102) and PC5
 * (P27 on BRD4001 J102) as input.
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
#include "em_burtc.h"
#include "em_iadc.h"
#include "em_ldma.h"
#include "em_gpio.h"
#include "mx25flash_spi.h"
#include "bsp.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define POWER_DOWN_RAM  (1)

// How many samples to capture
#define NUM_SAMPLES               1024

// Set CLK_ADC to 1MHz
#define CLK_SRC_ADC_FREQ          5000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ              1000000 // CLK_ADC

// Set IADC timer cycles
#define TIMER_CYCLES              50000  // 50000 => 100   samples/second
                                         // 5000  => 1000  samples/second
                                         // 1000  => 5000  samples/second
                                         // 500   => 10000 samples/second
                                         // 200   => 25000 samples/second

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

/// Globally declared LDMA link descriptor
LDMA_Descriptor_t descriptor;

// buffer to store IADC samples
uint32_t singleBuffer[NUM_SAMPLES];

/**************************************************************************//**
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO (void)
{
  // Enable GPIO clock branch
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure push button PB0 as a user input; will use as a toggle to indicate when inputs are ready
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Configure LED0 as output, will be set when LDMA transfer completes
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

  // Configure IADC clock source for use while in EM2
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO); // 20MHz

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
  initSingle.dataValidLevel = iadcFifoCfgDvl4;

  // Enable triggering of single conversion
  initSingle.start = true;

  // Set to run in EM2
  initSingle.fifoDmaWakeup = true;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = IADC_INPUT_0_PORT_PIN;
  initSingleInput.negInput = iadcNegInputGnd;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Single
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC;
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
  // Declare LDMA init structs
  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  // Enable LDMA clock branch
  CMU_ClockEnable(cmuClock_LDMA, true);

  // Initialize LDMA with default configuration
  LDMA_Init(&init);

  // Configure LDMA for transfer from IADC to memory
  // LDMA will loop continuously
  LDMA_TransferCfg_t transferCfg =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_IADC0_IADC_SINGLE);

  // Set up descriptor for buffer transfer
  LDMA_Descriptor_t xfer = LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&IADC0->SINGLEFIFODATA, buffer, size, 0);

  // Store descriptors globally
  // Note that this is required for the LDMA to function properly
  descriptor = xfer;

  // Set descriptor to loop NUM_SAMPLES times; disables loop counting to run continuously
  descriptor.xfer.decLoopCnt = 0;
  descriptor.xfer.xferCnt = NUM_SAMPLES;

  // Interrupt after transfer complete
  descriptor.xfer.doneIfs = 1;
  descriptor.xfer.ignoreSrec = 0;

  // Start transfer, LDMA will sample the IADC NUM_SAMPLES time, and then interrupt
  LDMA_StartTransfer(0, (void*)&transferCfg, (void*)&descriptor);
}

/**************************************************************************//**
 * @brief  LDMA Handler
 *****************************************************************************/
void LDMA_IRQHandler(void)
{
  // Clear interrupt flags
  LDMA_IntClear(LDMA_IF_DONE0);

  // Re-enable GPIO clock branch (disabled prior to entering EM2)
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Toggle GPIO to notify that transfer is complete
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

  // Disable GPIO clock branch (shutting off unnecessary clock trees)
  CMU_ClockEnable(cmuClock_GPIO, false);
}

/***************************************************************************//**
 * @brief
 *   Enter EM2 with BURTC running on a low frequency oscillator.
 *
 * @param[in] osc
 *   Oscillator to run RTCC from (LFXO or LFRCO).
 ******************************************************************************/
void initBURTC(CMU_Select_TypeDef osc)
{
  // Route desired oscillator to BURTC clock tree
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFRCO);

  // Setup BURTC parameters
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.clkDiv = 256;

  // Initialize BURTC
  CMU_ClockEnable(cmuClock_BURTC, true);
  BURTC_Reset();
  BURTC_Init(&burtcInit);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  FlashStatus status;

  CHIP_Init();

  // Select FSRCO for SYSCLK; FSRCO is default out of reset
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);

  // Initialize GPIO
  initGPIO();

  // Debugging Catch; wait here before proceeding into EM2
  // Allows Energy Profiler to start up and show prior EM0 current
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) != PB_PRESSED); //user feedback
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == PB_PRESSED); //make it a toggle

  // Turn off GPIO input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 1);

  // Turn on DCDC regulator
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_WSTK_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Enable voltage downscaling in EM2/3 (VSCALE0)
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;

  // Initialize EM mode 2/3
  EMU_EM23Init(&em23Init);

#ifdef EM2DEBUG
#if (EM2DEBUG == 1)
  // Enable debug connectivity in EM2
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;
#endif
#endif

  // Initialize and power-down MX25 SPI flash
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  /* Initialize BURTC; Attempting to match datasheet current consumption test
   * condition - BURTC running from LFRCO; full RAM retention
   */
  initBURTC(cmuSelect_LFRCO);

  // Initialize the IADC
  initIADC();

  // Disable no longer needed clock trees for lowest energy consumption in EM2
  CMU_ClockEnable(cmuClock_GPIO, false);
  CMU_ClockEnable(cmuClock_MSC, false);
  CMU_ClockEnable(cmuClock_DCDC, false);

  // Initialize LDMA
  initLDMA(singleBuffer, NUM_SAMPLES);

  // IADC single already enabled; must enable timer block in order to trigger
  IADC_command(IADC0, iadcCmdEnableTimer);

  /* Infinite loop; once LDMA transfer completes, wake up, handle IRQ, and
   * return to EM2 until next LDMA transfer completes
   */
  while(1){
    // Sleep CPU until LDMA transfer completes
    EMU_EnterEM2(true);
  }
}
