/***************************************************************************//**
 * @file
 * @brief This project demonstrates the external clock single input mode of
 *        the pulse counter with interrupts. BTN0 is used as the pulse counter
 *        clock in this example. The program requests an interrupt whenever
 *        the pulse counter goes below zero. In this example, each toggle of
 *        PC5 will decrease the counter value by 1. The initial 
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
 *        This function acknowledges the interrupt request and toggles LED0
 ******************************************************************************/        
void PCNT0_IRQHandler(void)
{
  /* Acknowledge interrupt */
  PCNT_IntClear(PCNT0, PCNT_IFC_UF);

  /* Toggle LED0 */
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/***************************************************************************//**
 * @brief GPIO odd interrupt handler
 *        This function acknowledges odd pin number interrupt and toggles PC5
 *        output
 ******************************************************************************/        
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge all odd number pin interrupt */
  GPIO_IntClear(0xAAAA);

  /* Toggle PC5 */
  GPIO_PinOutToggle(gpioPortC, 5);
}

/***************************************************************************//**
 * @brief Initialize PCNT
 *        This function initializes pulse counter 0 and sets up the external
 *        single mode. PCNT underflow interrupt is configured in this also.
 ******************************************************************************/        
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeExtSingle;        // external single mode
  pcntInit.top      = 5;                        // Interrupt at every 6 PC5 toggle
  pcntInit.countDown = true;                    // counting down

  /* Init PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Enable interrupt */
  PCNT_IntEnable(PCNT0, PCNT_IEN_UF);

  /* route PCNT0 to location 1 where PC0 is */
  PCNT0 ->ROUTE = PCNT_ROUTE_LOCATION_LOC1;
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

  /* Configure pin I/O - PE0 */
  GPIO_PinModeSet(gpioPortE, 0, gpioModeInputPull, 0);

  /* Configure pin I/O - BTN0 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  /* Configure pin I/O - PC5 */
  GPIO_PinModeSet(gpioPortC, 5, gpioModePushPull, 0);

  /* Configure interrupt for BTN0 */
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 0, 1, true);
}

/***************************************************************************//**
 * @brief Initialize NVIC
 *        This function enables PCNT0 interrupt request in the interrupt 
 *        controller
 ******************************************************************************/        
static void setupNvic(void)
{
  /* Clear PCNT0 pending interrupt */
  NVIC_ClearPendingIRQ(PCNT0_IRQn);

  /* Clear odd GPIO pending interrupt */
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);

  /* Enable PCNT0 interrupt in the interrupt controller */
  NVIC_EnableIRQ(PCNT0_IRQn);

  /* Enable odd GPIO interrupt in the interrupt controller */
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Use LFRCO as LFA clock for LETIMER and PCNT */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_HFLE, true);

  /* Initialize GPIO */
  setupGpio();

  /* Initialize PCNT */
  setupPcnt();

  /* Initialize NVIC */
  setupNvic();

  /* Enter EM3 forever */
  while (true) {
    EMU_EnterEM3(false);
  }
}

