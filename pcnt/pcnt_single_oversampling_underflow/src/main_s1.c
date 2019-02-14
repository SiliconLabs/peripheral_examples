/***************************************************************************//**
 * @file
 * @brief This project demonstrates the single input oversampling mode of the
 *        pulse counter with interrupts. LFACLK is used as the pulse counter clock in 
 *        this example. The program generates an interrupt whenever the pulse counter
 *        goes below zero. In this example, each press of Push Button PB0 will decrease
 *        the counter value by 1. The initial value of the counter and the reload value
 *        from the top register is set by the user.
 * @version 5.5.0
 ********************************************************************************
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
  PCNT_IntClear(PCNT0, PCNT_IFC_UF);

  /* Toggle LED0 */
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/***************************************************************************//**
 * @brief PCNT0 setup
 *        This function sets up PCNT0 with oversampling single mode
 *        Counter and top value default to 5
 *        Event triggering when counting down
 ******************************************************************************/        
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;
  PCNT_Filter_TypeDef pcntFilterInit = PCNT_FILTER_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode      = pcntModeOvsSingle;
  pcntInit.counter   = 5;                       // Initial value of counter, interrupt every 6 press
  pcntInit.top       = 5;                       // reloading value of counter
  pcntInit.s1CntDir  = false;                   // S1 does not affect counter direction, countDown where determine the direction
  pcntInit.countDown = true;                    // counting down
  pcntInit.cntEvent  = pcntCntEventDown;        // event only triggered when counting down
  pcntInit.s0PRS     = pcntPRSCh0;
  pcntInit.filter    = true;                    // Filter GPIO to remove glitching LED behavior

  /* Use max filter len for GPIO push button */
  pcntFilterInit.filtLen = _PCNT_OVSCFG_FILTLEN_MASK;

  /* Enable PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Filter configuration */
  PCNT_FilterConfiguration(PCNT0, &pcntFilterInit, true);

  /* Enable PRS0 for PCNT0 */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  /* Enable underflow interrupt for PCNT0 */
  PCNT_IntEnable(PCNT0, PCNT_IEN_UF);
}

/***************************************************************************//**
 * @brief PRS setup
 *        This function sets up GPIO PRS pin 6 which links BTN0 to PCNT0 PRS0
 ******************************************************************************/        
static void setupPrs(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Set up GPIO PRS pin 6 */
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOL, PRS_CH_CTRL_SIGSEL_GPIOPIN6);
}

/***************************************************************************//**
 * @brief GPIO setup
 *        This function configures BTN0 as input and enables external interrupt
 *        for BTN0
 ******************************************************************************/        
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  /* Configure pin I/O - BTN0 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);

  /* Configure BTN0 for external interrupt */
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 6, false, false, false);
}

/***************************************************************************//**
 * @brief NVIC setup
 *        This function enables PCNT0 interupt requests in the
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

  /* GPIO Initilization */
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

