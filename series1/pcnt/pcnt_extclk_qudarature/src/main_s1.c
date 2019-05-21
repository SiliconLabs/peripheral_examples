/***************************************************************************//**
 * @file
 * @brief   This project demonstrates the external quadrature mode of the 
 *          pulse counter peripheral. The program will request an interrupt 
 *          whenever the quadrature decoder detects a change in direction.
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

/***************************************************************************//**
 * @brief PCNT0 interrupt handler
 *        This function acknowleges the interrupt and toggles LED0
 ******************************************************************************/        
void PCNT0_IRQHandler(void)
{
  PCNT_IntClear(PCNT0, PCNT_IFC_DIRCNG);
  if(PCNT0->STATUS & PCNT_STATUS_DIR)	                        // check if the direction is down
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);	// toggle led 0 if down
  else
    GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);	// toggle led 1 if up
}

/***************************************************************************//**
 * @brief PCNT0 setup
 *        This function sets up PCNT0 with exteral quadrature mode
 *        S0 PRS linked to PRSCH0
 *        S1 PRS linked to PRSCH1
 ******************************************************************************/
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeExtQuad;          // External qudrature mode
  pcntInit.s1CntDir = false;                    // S1 does not affect counter direction, default count up
  pcntInit.s0PRS    = pcntPRSCh0;
  pcntInit.s1PRS    = pcntPRSCh1;
  pcntInit.filter   = true;                     // Filter GPIO to remove glitching LED behavior

  /* PCNT0 Init */
  PCNT_Init(PCNT0, &pcntInit);

  /* Enable PRS0 input */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  /* Enable PRS1 input */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS1, true);

  /* Enable PCNT0 interrupt */
  PCNT_IntEnable(PCNT0, PCNT_IEN_DIRCNG);
}

/***************************************************************************//**
 * @brief PRS setup
 *        This function sets up GPIO PRS pin 6 and 7 which links
 *        BTN0 and BTN1 to PRS0 and PRS1
 ******************************************************************************/        
static void setupPrs(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Set up GPIO PRS pin 6 */
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOL, PRS_CH_CTRL_SIGSEL_GPIOPIN6);

  /* Set up GPIO PRS pin 7 */
  PRS_SourceAsyncSignalSet(1, PRS_CH_CTRL_SOURCESEL_GPIOL, PRS_CH_CTRL_SIGSEL_GPIOPIN7);
}

/***************************************************************************//**
 * @brief GPIO setup
 *        This function configures BTN0 and BTN1 as input and enables
 *        external interrupt for BTN0 and BTN1
 ******************************************************************************/        
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

  /* Configure pin I/O - BTN0 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  /* Configure pin I/O - BTN1 */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);

  /* Configure BTN0 for external interrupt */
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 6, false, false, false);

  /* Configure BTN1 for external interrupt */
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, 7, false, false, false);
}

/***************************************************************************//**
 * @brief NVIC setup
 *        This function enables PCNT0 interrupt requests in the
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

