/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the DCDC VREGVDD Threshold Comparator.
 * The device will be switching between DCDC regulator on and DCDC bypass mode
 * depending on the input voltage to VREGVDD. The LCD will display the DCDC
 * modes.
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
 ***************************   GLOBAL VARIABLES   *****************************
 *****************************************************************************/

// test mode control signals
volatile bool low_voltage = false;

/******************************************************************************
 * @brief Interrupt handler for push button BTN1.
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO->IF_CLR = GPIO->IF;
}

/**************************************************************************//**
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO (void)
{
  // Configure push button 0 as input, enable edge detection interrupt
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN,
      gpioModeInputPullFilter, 1);
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN,
      BSP_GPIO_PB0_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

/*****************************************************************************/
/* @brief: Starter kit LCD display, select voltage
 * to be monitored
 *****************************************************************************/
void displaySelection(void){

  // Welcome Message
  printf("EFR32xG22 VMON\nwith DCDC\nBuilt in Comparator"
      "\nSoftware Example\n\n\n");
}

/*****************************************************************************/
/*@brief: enable clock branches
 *****************************************************************************/
void initCMU(void){
  // Disable all low frequency clock except LFXO to save power
  CMU_OscillatorEnable(cmuOsc_ULFRCO,false,true);
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  CMU_OscillatorEnable(cmuOsc_LFRCO,false,true);
  CMU_OscillatorEnable(cmuOsc_FSRCO,false,true);

  // Select all low frequency clock to LFXO
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
  CMU_ClockSelectSet(cmuClock_WDOG0CLK, cmuSelect_LFXO);
  CMU_ClockSelectSet(cmuClock_RTCCCLK, cmuSelect_LFXO);

  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);
}

/*****************************************************************************/
/*@brief: DCDC interrupt handler
 *****************************************************************************/
void DCDC_IRQHandler(void){
  volatile uint32_t flag = DCDC -> IF;

  // Clear all interrupt flag
  DCDC -> IF_CLR = flag;

  // Check for interrupt flag
  if(flag & DCDC_IEN_VREGINLOW){
    low_voltage = true;
    DCDC -> IEN_CLR |= DCDC_IEN_VREGINLOW;
  }
  if(flag & DCDC_IEN_VREGINHIGH){
    low_voltage = false;
    DCDC -> IEN_CLR |= DCDC_IEN_VREGINHIGH;
  }
}

/*****************************************************************************/
/*@brief DCDC Initialization
 *****************************************************************************/
void initDCDC(void){
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;

  // Change dcdc threshold voltage level to 2.2V
  dcdcInit.cmpThreshold = emuVreginCmpThreshold_2v2;

  printf("DCDC Initializing\n");
  printf("Raise VREGVDD above 2.2V to start DCDC\n");
  RETARGET_SerialFlush(); // delay for printf to finish

  // Initializing DCDC with regulator on
  EMU_DCDCInit(&dcdcInit);

  // Initialization success message
  printf("DCDC initialization successful, running with regulator enabled\n");
  printf("Decrease VREGVDD supply to below 2.2V to enter bypass mode\n");
  RETARGET_SerialFlush(); // delay for printf to finish

  // Clear all DCDC interrupt
  DCDC->IF_CLR |= _DCDC_IF_MASK;

  // Enable VREGIN low detection interrupt
  DCDC->IEN_SET |= DCDC_IEN_VREGINLOW;

  // Enable DCDC interrupt vector.
  NVIC_ClearPendingIRQ(DCDC_IRQn);
  NVIC_EnableIRQ(DCDC_IRQn);
}

/*****************************************************************************/
/*@brief: EMU Init routine, initialize EM23 with default settings
 *****************************************************************************/
void initEMU(void){
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_EM23Init(&em23Init);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // initialize peripheral clocks
  initCMU();

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

  // Initialize GPIO
  initGPIO();

  // Initialize EM2/3 with default settings
  initEMU();

  // Initialize DCDC
  initDCDC();

  // Disable GPIO interrupt
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN,
      false, true, false);

  // Infinite loop
  while(1){

    // Enter EM1 while waiting for threshold detection
    EMU_EnterEM1();

    // If below threshold, switch to bypass mode
    // Enable interrupt flags to detect VREGIN > VTHRESHOLD
    if(low_voltage){
      printf("VREGVDD below threshold voltage switching to bypass mode\n");
      RETARGET_SerialFlush(); // delay for printf to finish
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      printf("Bypass mode enabled. Raise VREGVDD to 2.2V to enable DCDC\n");
      RETARGET_SerialFlush(); // delay for printf to finish
      DCDC -> IEN_SET |= DCDC_IEN_VREGINHIGH;
    }

    // If above threshold, switch to regulator on mode
    // Enable interrupt flags to detect VREGIN < VTHRESHOLD
    else{
      EMU_DCDCModeSet(emuDcdcMode_Regulation);
      printf("DCDC enabled; Decrease VREGVDD supply to below 2.2V to enter bypass mode\n");
      RETARGET_SerialFlush(); // delay for printf to finish
      DCDC -> IEN_SET |= DCDC_IEN_VREGINLOW;
    }
  }
}
