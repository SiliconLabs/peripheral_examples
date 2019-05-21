/***************************************************************************//**
 * @file
 * @brief  This project demonstrates the external quadrature mode of the
 *         pulse counter peripheral. The program will request an interrupt
 *         whenever the quadrature decoder detects a change in direction.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_pcnt.h"
#include "em_prs.h"

#include "bsp.h"

#include <stdint.h>
#include <stdbool.h>

/* Global Variables */
int setflag1 = 1;
int setflag2 = 1;

/***************************************************************************//**
 * @brief PCNT0 interrupt handler
 *        This function acknowledges the interrupt and toggles LED
 ******************************************************************************/
void PCNT0_IRQHandler(void)
{
  /* Acknowledge interrupt */
  PCNT_IntClear(PCNT0, PCNT_IFC_DIRCNG);

  /* Check direction */
  if(PCNT0 -> STATUS & PCNT_STATUS_DIR) {
    if(setflag1 == 1){
      BSP_LedSet(0);	        // set LED0
      setflag1 = 0;             // clear LED0 on next interrupt
    }
    else{
      BSP_LedClear(0);          // clear LED0
      setflag1 = 1;             // set LED0 on next interrupt
    }
  }
  else {
    if(setflag2 == 1){
      BSP_LedSet(1);            // set LED1
      setflag2 = 0;             // clear LED1 on next interrupt
    }
    else{
      BSP_LedClear(1);          // clear LED1
      setflag2 = 1;             // set LED1 on next interrupt
    }
  }
}

/***************************************************************************//**
 * @brief PCNT0 setup
 *        This function sets up PCNT0 with external quadrature mode
 *        S0 PRS linked to PRSCH0
 *        S1 PRS linked to PRSCH1
 *        Enabled filter
 ******************************************************************************/        
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeExtQuad;          // External quadrature mode
  pcntInit.top      = 5;
  pcntInit.s0PRS    = pcntPRSCh0;
  pcntInit.s1PRS    = pcntPRSCh1;
  pcntInit.filter   = true;                     // Filter GPIO to remove glitching LED behavior

  /* Enable PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Enable PRS0 for PCNT0 */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  /* Enable PRS1 for PCNT0 */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS1, true);

  /* Enable direction change interrupt for PCNT0 */
  PCNT_IntEnable(PCNT0, PCNT_IEN_DIRCNG);
}

/***************************************************************************//**
 * @brief PRS setup
 *        This function sets up GPIO PRS pin 8 and 9 which links
 *        BTN0 and BTN1 to PCNT0 PRS0 and PCNT0 PRS1
 ******************************************************************************/        
static void setupPrs(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Set up GPIO PRS pin 8 */
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN8);

  /* Set up GPIO PRS pin 9 */
  PRS_SourceAsyncSignalSet(1, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN9);
}

/***************************************************************************//**
 * @brief GPIO setup
 *        This function configures BTN0 and BTN1 as inputs and enables external
 *        interrupts for BTN0 and BTN1
 ******************************************************************************/        
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1);

  /* Configure pin I/O - BTN0 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);

  /* Configure pin I/O - BTN1 */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 1);

  /* Configure BTN0 for external interrupt */
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 8, false, false, false);

  /* Configure BTN1 for external interrupt */
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, 9, false, false, false);
}

/***************************************************************************//**
 * @brief NVIC setup
 *        This function enables PCNT0 interrupts request in the
 *        interrupt controller
 ******************************************************************************/        
static void setupNvic(void)
{
  /* Clear PCNT0 pending interrupt */
  NVIC_ClearPendingIRQ(PCNT0_IRQn);

  /* Enable PCNT0 interrupt in the interrupt controller */
  NVIC_EnableIRQ(PCNT0_IRQn);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);

  /* Use LFRCO as LFA clock for LETIMER and PCNT */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_HFLE, true);

  /* GPIO Initialization */
  setupGpio();

  /* PCNT Initialization */
  setupPcnt();

  /* PRS Initialization */
  setupPrs();

  /* NVIC Initialization */
  setupNvic();

  /* Enter EM2 forever */
  while (true) {
    EMU_EnterEM2(false);
  }
}

