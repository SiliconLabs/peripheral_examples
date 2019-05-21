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

/***************************************************************************//**
 * @brief PCNT0 interrupt handler
 *        This function acknowledges the interrupt and toggles LED0
 ******************************************************************************/        
void PCNT0_IRQHandler(void)
{
  /* Acknowledge interrupt */
  PCNT_IntClear(PCNT0, PCNT_IFC_DIRCNG);

  /* Check direction */
  if(PCNT0 -> STATUS & PCNT_STATUS_DIR) {
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);     // if direction down, toggle led 0
  }
  else {
    GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);     // if direction up, toggle led 1
  }
}

/***************************************************************************//**
 * @brief PCNT0 setup
 *        This function sets up PCNT0 with external quadrature mode
 ******************************************************************************/
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeExtQuad;             // External quadrature mode
  pcntInit.top      = 5;

  /* Enable PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Route to location 1 where PE0 and PE1 is located */
  PCNT0 -> ROUTE = PCNT_ROUTE_LOCATION_LOC1;

  /* Enable direction change interrupt */
  PCNT_IntEnable(PCNT0, PCNT_IEN_DIRCNG);
}

/***************************************************************************//**
 * @brief GPIO setup
 *        This function configures PE0 and PE1 as input
 ******************************************************************************/        
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

  /* Configure pin I/O - PE0, PE1 */
  GPIO_PinModeSet(gpioPortE, 0, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortE, 1, gpioModeInput, 0);
}

/***************************************************************************//**
 * @brief NVIC setup
 *        This function enables PCNT0 and odd pin GPIO interrupt requests in the
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
  /* Chip errata */
  CHIP_Init();

  /* Use LFRCO as LFA clock for LETIMER and PCNT */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_HFLE, true);

  /* GPIO Initialization */
  setupGpio();

  /* PCNT Initialization */
  setupPcnt();

  /* NVIC Initialization */
  setupNvic();

  /* Enter EM2 forever */
  while (true) {
    EMU_EnterEM2(false);
  }
}

