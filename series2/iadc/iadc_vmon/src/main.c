/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the usage of the window comparator feature
 * in the IADC to monitor supply voltage.
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
 
#include "stdio.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_iadc.h"
#include "em_gpio.h"
#include "bsp.h"
#include "retargetserial.h"
#include "mx25flash_spi.h"
/******************************************************************************
 *******************************   DEFINES   **********************************
 *****************************************************************************/

// Set CLK_ADC to 100kHz (this corresponds to a sample rate of 10ksps)
#define CLK_SRC_ADC_FREQ        10000000  // CLK_SRC_ADC
#define CLK_ADC_FREQ            100       // CLK_ADC

#define MENU_MAX      4

// Voltage selection enum
#define AVDD          1
#define AVDD_BYPASS   2
#define DVDD_BYPASS   3
#define DVDD          4

/******************************************************************************
 ***************************   GLOBAL VARIABLES   *****************************
 *****************************************************************************/

// Stores latest ADC sample
static volatile IADC_Result_t sample;

// test mode control signals
static volatile bool menuKey = false;
static volatile bool runKey = false;
static volatile bool detected_flag = false;
static volatile uint8_t menuLevel = 0;

// Upper and lower bound for Window Comparator
// 16-bit left-justified format; 12-bit conversion result compared to upper
// 12-bits of window comparator
static uint16_t avdd_lower_bound = 0x7550;  // 2.2V
static uint16_t avdd_upper_bound = 0xCAA0;  // 3.8V

static uint16_t avdd_lower_bound_bypass = 0x6550;   // 1.9V
static uint16_t avdd_upper_bound_bypass = 0xC540;   // 3.7V

static uint16_t dvdd_lower_bound = 0x5D90;  // 1.755V
static uint16_t dvdd_upper_bound = 0x6330;  // 1.86V

static uint16_t dvdd_lower_bound_bypass = 0x6550;   // 1.9V
static uint16_t dvdd_upper_bound_bypass = 0xC540;   // 3.7V

// voltage rail voltage range in mV
static volatile uint16_t dvdd_upper_voltage;
static volatile uint16_t dvdd_lower_voltage;

static volatile uint16_t avdd_upper_voltage;
static volatile uint16_t avdd_lower_voltage;

static volatile uint16_t avdd_upper_voltage_bypass;
static volatile uint16_t avdd_lower_voltage_bypass;

static volatile uint16_t dvdd_upper_voltage_bypass;
static volatile uint16_t dvdd_lower_voltage_bypass;

// measured voltage using ADC
static volatile uint16_t voltagemV;

/******************************************************************************
 * @brief Interrupt handler for push button BTN1.
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  // Clear interrupt flags
  GPIO->IF_CLR = GPIO->IF;

  // If menuKey not pressed, increase menu
  if (!menuKey)
    menuKey = true;
}

/******************************************************************************
 * @brief Interrupt handler for push button BTN0.
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO->IF_CLR = GPIO->IF;
  runKey = true;
}

/**************************************************************************//**
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO (void)
{
  // Configure LED0 as output; enable WSTK LED0
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  // Configure PB0 and PB1 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN,gpioModeInputPullFilter, 1);
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN,
      false, true, true);

  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN,gpioModeInputPullFilter, 1);
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN,
      false, true, true);

  // Enable falling edge detection interrupt for PB0 and PB1
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief  IADC Initializer
 *****************************************************************************/
void initIADC (IADC_PosInput_t posInput, IADC_NegInput_t negInput,
    uint16_t upperbound, uint16_t lowerbound)
{
  // Declare init structs
  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t initSingleInput = IADC_SINGLEINPUT_DEFAULT;

  // Reset IADC to reset configuration in case it has been modified
  IADC_reset(IADC0);

  // Configure IADC clock source to use the FSRCO(EM2/EM3)
  CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

  // Modify init structs and initialize
  init.warmup = iadcWarmupKeepWarm;

  // Set the clk_src_adc prescale value here
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);

  // Set upper bound for window compare
  init.greaterThanEqualThres = upperbound;

  // Set lower bound for window compare
  init.lessThanEqualThres = lowerbound;

  // Configuration 0 is used by both scan and single conversions by default
  // Use internal 1.2V bandgap as reference
  initAllConfigs.configs[0].reference = iadcCfgReferenceInt1V2;

  // Divides CLK_SRC_ADC to set the CLK_ADC frequency for desired sample rate
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                              CLK_ADC_FREQ,
                                                                         0,
                                                         iadcCfgModeNormal,
                                                       init.srcClkPrescale);

  // Set conversions to run continuously
  initSingle.triggerAction = iadcTriggerActionContinuous;

  // Configure Input sources for single ended conversion
  initSingleInput.posInput = posInput;
  initSingleInput.negInput = negInput;

  // Enable window comparisons on this input
  initSingleInput.compare = true;

  // Initialize IADC
  IADC_init(IADC0, &init, &initAllConfigs);

  // Initialize Scan
  IADC_initSingle(IADC0, &initSingle, &initSingleInput);

  // Enable interrupts on window comparison match
  IADC_enableInt(IADC0, IADC_IF_SINGLECMP);

  // Enable ADC interrupts
  NVIC_ClearPendingIRQ(IADC_IRQn);
  NVIC_EnableIRQ(IADC_IRQn);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void IADC_IRQHandler(void)
{
  IADC_clearInt(IADC0, IADC_IF_SINGLECMP);

  // Read most recent sample
  sample = IADC_readSingleResult(IADC0);
  voltagemV = (sample.data)*4*1200/4095;

  // stop ADC conversion, set detected flag to true
  IADC_command(IADC0, iadcCmdStopSingle);
  detected_flag = true;
}

/*****************************************************************************/
/*@brief: enable clock branches
 *****************************************************************************/
void initCMU(void)
{
  CMU_ClockEnable(cmuClock_IADC0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
}

/*****************************************************************************/
/*@brief: initialize global variables
 *****************************************************************************/
void initVariable(void)
{
  // AVDD upper and lower voltage threshold in mV with DCDC regulator on
  avdd_upper_voltage = (avdd_upper_bound>>4)*1200*4/4095;
  avdd_lower_voltage = (avdd_lower_bound>>4)*1200*4/4095;

  // DVDD upper and lower voltage threshold in mV with DCDC regulator on
  dvdd_upper_voltage = (dvdd_upper_bound>>4)*1200*4/4095;
  dvdd_lower_voltage = (dvdd_lower_bound>>4)*1200*4/4095;

  // AVDD upper and lower voltage threshold in mV with DCDC regulator bypassed
  avdd_upper_voltage_bypass = (avdd_upper_bound_bypass>>4)*1200*4/4095;
  avdd_lower_voltage_bypass = (avdd_lower_bound_bypass>>4)*1200*4/4095;

  // DVDD upper and lower voltage threshold in mV with DCDC regulator bypassed
  dvdd_upper_voltage_bypass = (dvdd_upper_bound_bypass>>4)*1200*4/4095;
  dvdd_lower_voltage_bypass = (dvdd_lower_bound_bypass>>4)*1200*4/4095;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // DCDC Initialization
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;

  // DCDC startup voltage at 2.2V
  dcdcInit.cmpThreshold = emuVreginCmpThreshold_2v2;

  // initialize peripheral clocks
  initCMU();

  // Initialize GPIO
  initGPIO();

  // Init and power-down MX25 SPI flash
  FlashStatus status;
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  // Init
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  // Display Welcome Message
  printf("Press Push Button 0 to view test modes\n");
  RETARGET_SerialFlush(); // delay for printf to finish
  // initialize global variables
  initVariable();

  // Infinite loop
  while(1){
    // if PB0 pressed, test modes are displayed
    if(menuKey){
      IADC_reset(IADC0);
      printf("Press 1 for AVDD Voltage Monitoring with DCDC Enabled\n");
      printf("-- Upper Voltage Bound: %dmV\n", avdd_upper_voltage);
      printf("-- Lower Voltage Bound: %dmV\n", avdd_lower_voltage);
      printf("Press 2 for AVDD Voltage Monitoring with DCDC Bypassed\n");
      printf("-- Upper Voltage Bound: %dmV\n", avdd_upper_voltage_bypass);
      printf("-- Lower Voltage Bound: %dmV\n", avdd_lower_voltage_bypass);
      printf("Press 3 for DVDD Voltage Monitoring with DCDC Bypassed\n");
      printf("-- Upper Bound: %dmV\n",dvdd_upper_voltage_bypass);
      printf("-- Lower Bound: %dmV\n",dvdd_lower_voltage_bypass);
      printf("Press 4 for DVDD Voltage Monitoring with DCDC Enabled\n");
      printf("-- Upper Bound: %dmV\n",dvdd_upper_voltage);
      printf("-- Lower Bound: %dmV\n",dvdd_lower_voltage);
      RETARGET_SerialFlush(); // delay for printf to finish

      // To select the test mode
      while(menuLevel<49 || menuLevel >53){
        menuLevel =  (uint8_t)RETARGET_ReadChar();
      }
      printf("Selected Test Mode: %d\n",menuLevel);
      printf("-- Press PB1 to run the test or Press PB0 to display test mode\n");
      RETARGET_SerialFlush(); // delay for printf to finish
      // set menuKey flag to false
      menuKey = false;
    }

    // if PB1 pressed, run the selected mode
    if(runKey){
      switch (menuLevel){
        case AVDD:
          dcdcInit.mode = emuDcdcMode_Regulation;
          printf("-- Starting DCDC\n");
          printf("-- Please raise input voltage to at least 2.2V to startup the DCDC\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          EMU_DCDCInit(&dcdcInit);  // initialize dcdc with regulator on
          initIADC(iadcPosInputAvdd,iadcNegInputGnd,avdd_upper_bound,avdd_lower_bound);
          IADC_command(IADC0, iadcCmdStartSingle);  // Start scan
          printf("-- running AVDD Voltage Monitor with DCDC Enabled\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          break;
        case AVDD_BYPASS:
          dcdcInit.mode = emuDcdcMode_Bypass; // initialize dcdc in bypass mode
          EMU_DCDCInit(&dcdcInit);
          initIADC(iadcPosInputAvdd,iadcNegInputGnd,avdd_upper_bound_bypass,avdd_lower_bound_bypass);
          IADC_command(IADC0, iadcCmdStartSingle);  // Start scan
          printf("-- running AVDD Voltage Monitor with DCDC Bypassed\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          break;
        case DVDD_BYPASS:
          dcdcInit.mode = emuDcdcMode_Bypass; // initialize dcdc in bypass mode
          EMU_DCDCInit(&dcdcInit);
          initIADC(iadcPosInputDvdd,iadcNegInputGnd,dvdd_upper_bound_bypass,dvdd_lower_bound_bypass);
          IADC_command(IADC0, iadcCmdStartSingle);  // Start scan
          printf("-- running DVDD Voltage Monitor with DCDC Bypassed\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          break;
        case DVDD:
          dcdcInit.mode = emuDcdcMode_Regulation;
          printf("-- Starting DCDC\n");
          printf("-- Please raise input voltage to at least 2.2V to startup the DCDC\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          EMU_DCDCInit(&dcdcInit);  // initialize dcdc with regulator on
          initIADC(iadcPosInputDvdd,iadcNegInputGnd,dvdd_upper_bound,dvdd_lower_bound);
          IADC_command(IADC0, iadcCmdStartSingle);  // Start scan
          printf("-- running DVDD Voltage Monitor with DCDC Enabled\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          break;
        default:
          break;
      }
        runKey = false;
    }

    // if compare mode triggered, update display,
    // power off DCDC if appropriate.
    if(detected_flag){
      switch(menuLevel){
        case AVDD:
          EMU_DCDCPowerOff();
          printf("-- DCDC enabled\n");
          printf("-- AVDD voltage out of range\n");
          printf("-- Expected Lower Voltage: %dmV\n", avdd_lower_voltage);
          printf("-- Expected Upper Voltage: %dmV\n", avdd_upper_voltage);
          printf("-- Voltage measured: %dmV\n\n", voltagemV);
          printf("-- Disabling DCDC\n");
          printf("Press PB0 to select mode or Press PB1 to run this mode again\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          break;
        case AVDD_BYPASS:
          printf("-- DCDC Bypassed\n");
          printf("-- AVDD voltage out of range\n");
          printf("-- Expected Lower Voltage: %dmV\n", avdd_lower_voltage_bypass);
          printf("-- Expected Upper Voltage: %dmV\n", avdd_upper_voltage_bypass);
          printf("-- Voltage measured: %dmV\n\n", voltagemV);
          printf("Press PB0 to select mode or Press PB1 to run this mode again\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          break;
        case DVDD_BYPASS:
          printf("-- DCDC Bypassed\n");
          printf("-- DVDD voltage out of range\n");
          printf("-- Expected Lower Voltage: %dmV\n", dvdd_lower_voltage_bypass);
          printf("-- Expected Upper Voltage: %dmV\n", dvdd_upper_voltage_bypass);
          printf("-- Voltage measured:\n%dmV\n\n", voltagemV);
          printf("Press PB0 to select mode or Press PB1 to run this mode again\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          break;
        case DVDD:
          printf("-- DCDC enabled\n");
          printf("-- DVDD voltage out of range\n");
          printf("-- Expected Lower Voltage: %dmV\n", dvdd_lower_voltage);
          printf("-- Expected Upper Voltage: %dmV\n", dvdd_upper_voltage);
          printf("-- Voltage measured:\n%dmV\n\n", voltagemV);
          printf("Press PB0 to select mode or Press PB1 to run this mode again\n");
          RETARGET_SerialFlush(); // delay for printf to finish
          break;
        default:
          break;
      }
      detected_flag = false;
    }

    // Enter EM3, wait for next interrupt
    EMU_EnterEM3(true);
  }
}
