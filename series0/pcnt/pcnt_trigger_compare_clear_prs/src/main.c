/***************************************************************************//**
 * @file
 * @brief This project demonstrates the tigger compare and clear functionality
 *         of the pulse counter using interrupts. The program requests an interrupt 
 *         whenever the pulse counter clock (PRS in this example) triggers a
 *         compare and clear command and the result is the counter less than 
 *         or equal to the top value. Note the top value is set to 5 in this
 *         example. This feature is only for series 1 boards.
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
 *        This function acknowledges the interrupt and toggles LED0
 ******************************************************************************/        
void PCNT0_IRQHandler(void)
{
  /* Acknowledge interrupt */
  PCNT_IntClear(PCNT0, PCNT_IFC_TCC);

  /* Toggle LED0 */
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/***************************************************************************//**
 * @brief PCNT0 setup
 *        This function sets up PCNT0 with trigger and compare mode.
 *        Top value default to 5.
 *        PRSCH1 trigger compare and clear input.
 *        trigger compare clear mode set to compare less than or equal to.
 *        PCNT0 counter mode is oversampling single.
 *******************************************************************************/        
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;
  PCNT_Filter_TypeDef pcntFilterInit = PCNT_FILTER_DEFAULT;
  PCNT_TCC_TypeDef pcntTccInit = PCNT_TCC_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeOvsSingle;
  pcntInit.top      = 5;                        // Interrupt at every 6 BTN0 press
  pcntInit.s1CntDir = false;                    // S1 does not affect counter direction, default count up
  pcntInit.s0PRS    = pcntPRSCh0;
  pcntInit.filter   = true;                     // Filter GPIO to remove glitching LED behavior

  pcntTccInit.mode    = tccModePRS;             // use PRS as tcc input
  pcntTccInit.tccPRS  = pcntPRSCh1;             // PRSCh1 as input
  pcntTccInit.compare = tccCompLTOE;            // interrupt trigger under condition counter <= top

  /* Use max filter len for GPIO push button */
  pcntFilterInit.filtLen = _PCNT_OVSCFG_FILTLEN_MASK;

  /* Enable PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Enable PCNT Trigger compare and clear */
  PCNT_TCCConfiguration(PCNT0, &pcntTccInit);

  /* Enable filter */
  PCNT_FilterConfiguration(PCNT0, &pcntFilterInit, true);

  /* Enable PRS0 for PCNT0 */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  /* Enable PRS1 for PCNT0 */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS1, true);

  /* Enable direction change interrupt for PCNT0 */
  PCNT_IntEnable(PCNT0, PCNT_IEN_TCC);
}

/***************************************************************************//**
 * @brief PRS setup
 *        This function sets up GPIO PRS pin 6 and pin 7 which link BTN0 and
 *        BTN1 to PCNT0 PRS0 and PCNT0 PRS1
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
 *        external interrupts for BTN0 and BTN1
 ******************************************************************************/        
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  /* Configure pin I/O - BTN0 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);

  /* Configure BTN0 for external interrupt */
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 6, false, false, false);

  /* Configure pin I/O - BTN1 */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 1);

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

