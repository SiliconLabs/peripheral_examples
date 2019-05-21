/**************************************************************************//**
 * @file
 * @brief This project demonstrates four different reset sources - POR, EM4, 
 * pin reset and system reset request. GPIO Push Buttons (PB0 & PB1) and LEDs 
 * (LED0 & LED1) are used in initiate and indicate different resets 
 * respectively. A specific pattern of LEDs indicates a specific reset source.
 * See readme.txt for details.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "em_cryotimer.h"
#include "em_burtc.h"
#include "em_gpio.h"
#include "em_chip.h"

#include "bspconfig.h"


// Global Varibles
unsigned long resetCause;
volatile uint32_t msTicks; // counts 1ms timeTicks 

// Function Declarations 
void initGPIO(void);
void configEM4(void);
void startBURTC(void);

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       // increment counter necessary in Delay()
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even pins
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  // Get and clear all pending GPIO interrupts
  GPIO_IntClear(GPIO_IntGet());

  // Configure EM4 for timer wakeup
  configEM4();

  //Configure and start BURTC to run while in EM4
  startBURTC();

  // Go into EM4 and wait for wakeup; LED(s) will be off
  GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
  GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
  EMU_EnterEM4();
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for odd pins
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  // Get and clear all pending GPIO interrupts
  GPIO_IntClear(GPIO_IntGet());

  // PB1: Initiate System reset request 
  NVIC_SystemReset();
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void){
  // Initialize chip 
  CHIP_Init();

  //Enable necessary clocks
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure the Push Buttons and the LEDs 
  initGPIO();

  // Store the cause of the last reset, and clear the reset cause register 
  resetCause = RMU_ResetCauseGet();
  // Clear Reset causes so we know which reset matters the next time 
  RMU_ResetCauseClear();

  // Setup SysTick Timer for 1 msec interrupts  
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

  // Enter loop, and wait for wdog reset 
  while (1)
  {
    // Check if Power on Reset (POR) triggered the last reset 
    if (resetCause & RMU_RSTCAUSE_PORST)
    {
      // Turn ON LED0 
      GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

      // Turn OFF LED1 
      GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    }

    // Check if entering/exiting EM4 triggered the last reset
    else if (resetCause & RMU_RSTCAUSE_EM4WURST)
    {
      // Turn ON LED0
      GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

      // Turn ON LED1
      GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    }
    // Check if external reset (pin reset) triggered the last reset 
    else if (resetCause & RMU_RSTCAUSE_EXTRST)
    {
      // Turn OFF LED0 
      GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

      // Turn ON LED1 
      GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    }
    // Check if system reset request triggered the last reset 
    else if (resetCause & RMU_RSTCAUSE_SYSREQRST)
    {
      // Toggle the LEDs together at 10 kHz
      GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
      GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
      Delay(100);
    }
  }
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure the LEDs as output pins
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

  // Configure PB0 as input and enable interrupt
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  // Configure PB1 as input and enable interrupt  
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  // Enable GPIO_EVEN interrupt
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  // Enable GPIO_ODD interrupt 
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief  Initialize EM4
 *****************************************************************************/
void configEM4(void)
{
    //Load EM4 defaults with kit specific parameters
    EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
    //Configure EM4 to be woken by BURTC interrupt
    em4Init.lockConfig    = true;
    em4Init.osc           = emuEM4Osc_ULFRCO;
    em4Init.buRtcWakeup   = true;
    em4Init.vreg          = true;

    EMU_EM4Init(&em4Init);
}

/**************************************************************************//**
 * @brief  Initialize and Start BURTC
 *****************************************************************************/
void startBURTC(void)
{
  // Enable access to BURTC registers
  RMU_ResetControl(rmuResetBU, false);
  BURTC_Enable(true);

  //Configure BURTC to run in EM4
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.mode      = burtcModeEM4;
  burtcInit.clkSel    = burtcClkSelULFRCO;

  //set BURTC to use 1kHz ULFCO
  burtcInit.clkDiv    = burtcClkDiv_2;

  //BURTC will interrupt after 1 second
  BURTC_CompareSet(0, 1000);

  //Enable Interrupt
  BURTC_IntEnable( BURTC_IF_COMP0 );
  BURTC_Init(&burtcInit);
}
