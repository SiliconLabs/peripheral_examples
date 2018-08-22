/***************************************************************************//**
 * @file
 * @brief This project demonstrates the external clock single input mode of
 *        the pulse counter with interrupts. BTN0 is used as the pulse counter
 *        clock in this example. The program requests an interrupt whenever
 *        the pulse counter goes below zero. In this example, each press of
 *        Push Button PB0 will decrease the counter value by 1. The initial 
 *        value of the counter and the reload value from the top register
 *        is set by the user.
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
 *        This function acknowleges the interrupt request and toggles LED0
 ******************************************************************************/        
void PCNT0_IRQHandler(void)
{
  /* Acknowledge interrupt */
  PCNT_IntClear(PCNT0, PCNT_IFC_UF);

  /* Toggle LED0 */
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/***************************************************************************//**
 * @brief Initialize PCNT
 *        This function initializes pulse counter 0 and sets up the external
 *        single mode
 *        PCNT0 underflow interrupt is configured in this function also
 *******************************************************************************/        
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);

  pcntInit.mode     = pcntModeExtSingle;        // External single mode
  pcntInit.top      = 5;                        // top counter overflow after 6 presses
  pcntInit.countDown = true;                    // counter counting down
  pcntInit.cntEvent = pcntCntEventDown;         // event triggering when counting down
  pcntInit.s1CntDir = false;                    // S1 does not affect counter direction
  pcntInit.s0PRS    = pcntPRSCh0;

  /* Init PCNT */
  PCNT_Init(PCNT0, &pcntInit);

  /* Enable PRS Channel */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  /* Enable Underflow Interrupt */
  PCNT_IntEnable(PCNT0, PCNT_IEN_UF);
}

/***************************************************************************//**
 * @brief Initialize PRS
 *        This function sets up the PRS to GPIO pin 6, which is used to wire PF6
 *        to PCNT0 PRS0
 ******************************************************************************/        
static void setupPrs(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Link PRS Channel 0 to GPIO PIN 6 */
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOL, PRS_CH_CTRL_SIGSEL_GPIOPIN6);
}

/***************************************************************************//**
 * @brief Initialize GPIO
 *        This function initializes push button PB0 and enables external interrupt
 *        for PRS functionality
 ******************************************************************************/        
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  /* Configure pin I/O - BTN0 on PF6 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);

  /* Configure external interrupt for BTN0 */
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 6, false, false, false);
}

/***************************************************************************//**
 * @brief Initialize NVIC
 *        This function enables PCNT0 interrupt request in the
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

  /* Initialize GPIO */
  setupGpio();

  /* Initialize PCNT */
  setupPcnt();

  /* Initialize PRS */
  setupPrs();

  /* Initialize NVIC */
  setupNvic();

  /* Enter EM3 forever */
  while (true) {
    EMU_EnterEM3(false);
  }
}

