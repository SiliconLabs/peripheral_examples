/***************************************************************************//**
 * @file main_single_low_current_xG22.c
 * @brief Uses the internal IADC timer to trigger a single-ended conversion,
 * which in turn triggers the LDMA to transfer the IADC measurement to memory,
 * all while remaining in EM2. After NUM_SAMPLES conversions the LDMA will
 * trigger an interrupt from EM2 and turn on LED1 on the WSTK.
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
 
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rtcc.h"
#include "em_iadc.h"
#include "em_ldma.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "mx25flash_spi.h"
#include "bsp.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// How many samples to capture
#define NUM_SAMPLES               1024

// Set CLK_ADC to 1 MHz (set to max for shortest IADC conversion/power-up time)
#define CLK_SRC_ADC_FREQ          5000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ              1000000 // CLK_ADC

// Set IADC timer cycles
#define TIMER_CYCLES              50000  // 50000 => 100   samples/second
                                         // 5000  => 1000  samples/second
                                         // 1000  => 5000  samples/second
                                         // 500   => 10000 samples/second
                                         // 200   => 25000 samples/second

// When changing IADC input port/pins, make sure to change xBUSALLOC macro's
// accordingly.
#define IADC_INPUT_0_BUS          CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0

// Push-buttons are active-low
#define PB_PRESSED (0)

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

// Globally declared LDMA link descriptor
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
  // Use unbuffered AVDD as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency
  // Default oversampling (OSR) is 2x, and Conversion Time = ((4 * OSR) + 2) / fCLK_ADC
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                    CLK_ADC_FREQ,
                                                                    0,
                                                                    iadcCfgModeNormal,
                                                                    init.srcClkPrescale);

  // Single initialization
  initSingle.triggerSelect = iadcTriggerSelTimer;
  initSingle.dataValidLevel = _IADC_SINGLEFIFOCFG_DVL_VALID1;

  // Enable triggering of single conversion
  initSingle.start = true;

  // Set to run in EM2
  initSingle.fifoDmaWakeup = true;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = iadcPosInputPortCPin4;
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
 *   IADC Initializer
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

  // Set up descriptors for dual buffer transfer
  LDMA_Descriptor_t xfer = LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&IADC0->SINGLEFIFODATA, buffer, size, 1);

  // Store descriptors globally
  // Note that this is required for the LDMA to function properly
  descriptor = xfer;

  // Set descriptor to loop NUM_SAMPLES times and then complete
  descriptor.xfer.decLoopCnt = 1;
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

  // Stop the IADC
  IADC_command(IADC0, iadcCmdStopSingle);

  // Re-enable GPIO clock branch (disabled when entering EM2)
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set GPIO to notify that transfer is complete
  GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/***************************************************************************//**
 * @brief   Disable high frequency clocks
 ******************************************************************************/
static void disableHFClocks(void)
{
  // Disable high frequency peripherals

  CMU_ClockEnable(cmuClock_USART0, true);

  USART0->EN_CLR = 0x1;

  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);

  // Check that HFRCODPLL and HFXO are not requested
  CMU->CLKEN0_SET = CMU_CLKEN0_HFRCO0;
  while (((HFRCO0->STATUS & _HFRCO_STATUS_ENS_MASK) != 0U)
         || ((HFXO0->STATUS & _HFXO_STATUS_ENS_MASK) != 0U)) {
  }

  CMU_ClockEnable(cmuClock_USART0, false);
  CMU_ClockEnable(cmuClock_PRS, false);
  CMU_ClockEnable(cmuClock_GPIO, false);
  CMU_ClockEnable(cmuClock_HFXO, false);
  CMU_ClockEnable(cmuClock_DPLL0, false);
  CMU_ClockEnable(cmuClock_HFRCO0, false);
  CMU_ClockEnable(cmuClock_MSC, false);
  CMU_ClockEnable(cmuClock_DCDC, false);
}

/***************************************************************************//**
 * @brief   Disable low frequency clocks
 ******************************************************************************/
static void disableLFClocks(void)
{
  // Disable low frequency peripherals

  CMU_ClockEnable(cmuClock_RTCC, true);

  RTCC->EN_CLR = 0x1;

  CMU_ClockEnable(cmuClock_LFRCO, true);

  // Check that all low frequency oscillators are stopped
  CMU->CLKEN0_SET = CMU_CLKEN0_LFRCO;
  while ((LFRCO->STATUS != 0U) && (LFXO->STATUS != 0U)) {
  }

  CMU_ClockEnable(cmuClock_RTCC, false);
  CMU_ClockEnable(cmuClock_LFRCO, false);
  CMU_ClockEnable(cmuClock_LFXO, false);
}

/***************************************************************************//**
 * @brief   Disable all clocks to achieve lowest current consumption numbers.
 ******************************************************************************/
static void disableClocks(void)
{
  // Disable High Frequency Clocks
  disableHFClocks();

  // Disable Low Frequency Clocks
  disableLFClocks();
}

/***************************************************************************//**
 * @brief
 *   Enter EM2 with RTCC running on a low frequency oscillator.
 *
 * @param[in] osc
 *   Oscillator to run RTCC from (LFXO or LFRCO).
 * @param[in] powerdownRam
 *   Power down all RAM except the first 16 kB block or retain full RAM.
 *
 * @details
 *   Parameter:
 *     EM2. Deep Sleep Mode.@n
 *   Condition:
 *     RTCC, 32.768 kHz LFXO or LFRCO.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM2_RTCC(CMU_Select_TypeDef osc, bool powerdownRam)
{
  // Make sure clocks are disabled.
  disableClocks();

  // Route desired oscillator to RTCC clock tree.
  CMU_ClockSelectSet(cmuClock_RTCCCLK, osc);

  // Setup RTC parameters
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  rtccInit.presc = rtccCntPresc_256;
  rtccInit.cntWrapOnCCV1 = true;

  // Initialize RTCC
  CMU_ClockEnable(cmuClock_RTCC, true);
  RTCC_Reset();
  RTCC_Init(&rtccInit);

  // Power down all RAM blocks except block 0
  if (powerdownRam) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Enter EM2.
  EMU_EnterEM2(false);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  FlashStatus status;

  // Use default settings for EM23 and HFXO
  EMU_EM01Init_TypeDef em01Init = EMU_EM01INIT_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_WSTK_DEFAULT;
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;

  CHIP_Init();

  initGPIO();

  // Debugging Catch; wait here before proceeding into EM2
  // Allows Energy Profiler to start up and show prior EM0 current
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) != PB_PRESSED); //user feedback
  while(GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == PB_PRESSED); //make it a toggle

  // Turn off GPIO input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 1);

  /* Turn on DCDC regulator */
  EMU_DCDCInit(&dcdcInit);

  EMU_UnlatchPinRetention();
  CMU_HFXOInit(&hfxoInit);

//  CMU_ClkOutPinConfig(2, cmuSelect_FSRCO, 1, gpioPortB, 2);

  /* Enable voltage downscaling in EM modes 0 to 3. */
  em01Init.vScaleEM01LowPowerVoltageEnable = true;
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;

  // Initialize EM2/EM3/EM4 with default parameters
  EMU_EM01Init(&em01Init);
  EMU_EM23Init(&em23Init);

  /* Init and power-down MX25 SPI flash */
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  // Initialize the IADC
  initIADC();

  // Initialize LDMA
  initLDMA(singleBuffer, NUM_SAMPLES);

  // IADC single already enabled; must enable timer block in order to trigger
  IADC_command(IADC0, iadcCmdEnableTimer);

  // Sleep CPU until LDMA transfer completes
  // EM2 with RTCC running off LFRCO is a documented current mode in the DS
  em_EM2_RTCC(cmuSelect_LFRCO, false);

  // Infinite loop
  while(1);
}
