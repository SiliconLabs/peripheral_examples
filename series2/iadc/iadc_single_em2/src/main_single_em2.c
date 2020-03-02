/**************************************************************************//**
 * @file
 * @brief Uses the IADC repeated nonblocking measurements on a single input
 * while asleep. Sample rate is 1 ksps, and the ADC reads GPIO pins PC4
 * (P25 on BRD4001 J102) as input.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *
 ******************************************************************************/

/*
This project demonstrates using the IADC peripheral to continuously take a single-ended
analog measurement while running in low power EM2. IADC interrupts on conversion completion
wake the MCU into EM0, where the IADC interrupt handler converts the result to
a voltage before going back into EM2. The IADC sampling rate is 1 ksps, and the 
ADC reads GPIO pin PC04 (J102 of BRD4001, Pin 25) as input. The IADC interrupt handler
also toggles GPIO PB01 (EXP13, WSTK P10) with each conversion.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "sample" and "singleResult" to the 
Expressions Window
4. Observe GPIO PB01 using an oscilloscope; toggles occur on every conversion, output
appears as 500HZ, 50% duty cycle clock
5. Suspend the debugger, observe the measured voltage change in the Expressions Window
and how it responds to different voltage values on the corresponding pins (see below)

Peripherals Used:
CLK_CMU_ADC  - 1 MHz HFRCOEM23 for Series 2
CLK_SRC_ADC  - 1 MHz for Series 2
CLK_ADC      - 10 kHz for Series 2
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in warmup mode
			 - Conversions initiated by firmware and triggered continuously
			   (when a conversion completes a new one is requested immediately without 
			   requiring a new trigger)

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB01 - GPIO Push/Pull output, Expansion Header Pin 13, WSTK Pin 10, LED1
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25
*/
 
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_iadc.h"
#include "em_gpio.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Set HFRCOEM23 to lowest frequency (1MHz)
#define HFRCOEM23_FREQ          cmuHFRCOEM23Freq_1M0Hz

// Set CLK_ADC to 10kHz (this corresponds to a sample rate of 1ksps)
#define CLK_SRC_ADC_FREQ        1000000 // CLK_SRC_ADC
#define CLK_ADC_FREQ            10000   // CLK_ADC

// When changing GPIO port/pins above, make sure to change xBUSALLOC macro's
// accordingly.
#define IADC_INPUT_BUS          CDBUSALLOC
#define IADC_INPUT_BUSALLOC     GPIO_CDBUSALLOC_CDEVEN0_ADC0

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   ******************************
 ******************************************************************************/

// Stores latest ADC sample and converts to volts
static volatile IADC_Result_t sample;
static volatile double singleResult;

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
  // Note that HFRCOEM23 is the lowest frequency source available for the IADC
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_HFRCOEM23);

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;

  // Set the HFSCLK prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Configuration 0 is used by both scan and single conversions by default
  // Use unbuffered AVDD as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                    CLK_ADC_FREQ,
                                                                    0,
                                                                    iadcCfgModeNormal,
                                                                    init.srcClkPrescale);

  // Single initialization
  initSingle.dataValidLevel = _IADC_SCANFIFOCFG_DVL_VALID1;

  // Set conversions to run continuously
  initSingle.triggerAction = iadcTriggerActionContinuous;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = iadcPosInputPortCPin4;
  initSingleInput.negInput = iadcNegInputGnd;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Scan
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Allocate the analog bus for ADC0 inputs
  GPIO->IADC_INPUT_BUS |= IADC_INPUT_BUSALLOC;

  // Enable interrupts on data valid level
  IADC_enableInt(IADC0, IADC_IF_SINGLEFIFODVL);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void IADC_IRQHandler(void)
{
  //GPIO_PinOutToggle(gpioPortB, 1);

  // Read data from the FIFO
  sample = IADC_pullSingleFifoResult(IADC0);

  // For single-ended the result range is 0 to +Vref, i.e., 12 bits for the
  // conversion value.
  singleResult = sample.data * 3.3 / 0xFFF;

  IADC_clearInt(IADC0, IADC_IF_SINGLEFIFODVL);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);
  //GPIO_PinModeSet(gpioPortB, 1, gpioModePushPull, 0);

  // Set clock frequency to defined value
  CMU_HFRCOEM23BandSet(HFRCOEM23_FREQ);

  // Initialize the IADC
  initIADC();

  // Start single
  IADC_command(IADC0, iadcCmdStartSingle);

  while (1)
  {
    // Enter EM2 sleep, woken by IADC interrupt
    EMU_EnterEM2(true);
  }
}
