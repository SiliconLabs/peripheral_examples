/***************************************************************************//**
 * @file
 * @brief This project demonstrates the single input oversampling mode using
 *        the LFA clock with interrupts. The program generates an interrupt 
 *        whenever the pulse counter detects the number of pulses that went
 *        above a threshold set by user. In this example, Push Button PB0 is
 *        used as the input source for toggling PC5. PC5 is used as the
 *        pulse counter clock source in this example.
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
  PCNT_IntClear(PCNT0, PCNT_IFC_OF);

  /* Toggle LED0 */
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/***************************************************************************//**
 * @brief Odd pin GPIO interrupt
 *        This function acknowledges odd pin GPIO interrupt and toggles PC5
 ******************************************************************************/        
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge all odd pin GPIO interrupt */
  GPIO_IntClear(0xAAAA);

  /* Toggle PC5 */
  GPIO_PinOutToggle(gpioPortC, 5);
}

/***************************************************************************//**
 * @brief PCNT0 setup
 *        This function sets up PCNT0 with oversampling single mode.
 *        Counter and top value defaul to 5
 *        Event triggering when counting up
 ******************************************************************************/        
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeOvsSingle;          // Oversampling single mode
  pcntInit.top      = 5;                          // Interrupt at every 6 BTN0 press
  pcntInit.counter  = 0;                          // Default counter value

  /* Enable PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Enable overflow interrupt for PCNT0 */
  PCNT_IntEnable(PCNT0, PCNT_IEN_OF);

  /* route PCNT0 to location 1 where PE0 is */
  PCNT0 ->ROUTE = PCNT_ROUTE_LOCATION_LOC1;
}

/***************************************************************************//**
 * @brief GPIO setup
 *        This function configures BTN0 as input and enable external interrupts
 *        for BTN0
 ******************************************************************************/        
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  /* Configure pin I/O - PE0 */
  GPIO_PinModeSet(gpioPortE, 0, gpioModeInputPull, 0);

  /* Configure pin I/O - BTM0 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  /* Configure pin I/O - PC5 */
  GPIO_PinModeSet(gpioPortC, 5, gpioModePushPull, 0);

  /* Enable BTN0 interrupt */
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 0, 1, true);
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

  /* Clear odd pin GPIO pending interrupt */
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);

  /* Enable PCNT0 interrupt in the interrupt controller */
  NVIC_EnableIRQ(PCNT0_IRQn);

  /* Enable odd pin GPIO interrupt in the interrupt controller */
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



