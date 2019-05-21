/**************************************************************************//**
 * @file main_gg11.c
 * @brief This project demonstrates the functionality of the watchdog
 * timer. It uses LEDs and a Push-button to indicate the state of
 * the system. If the LEDs are flashing, the system has not undergone
 * a reset. If the Push Button 0 is pressed and held for some time, it
 * will trigger a WDOG reset. See readme.txt for details.
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
#include "em_gpio.h"
#include "em_device.h"
#include "em_wdog.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_rmu.h"
#include "em_chip.h"
#include "em_system.h"
#include "bspconfig.h"

// GLOBAL VARIABLES 
unsigned long resetCause;
volatile uint32_t msTicks; // counts 1ms timeTicks 

// Function Declarations 
void initGPIO(void);
void initWDOG(void);

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
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata 
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  #if defined( _EMU_DCDCCTRL_MASK )
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  #endif


  // Store the cause of the last reset, and clear the reset cause register 
  resetCause = RMU_ResetCauseGet();
  // Clear Reset causes so we know which reset occurs the next time 
  RMU_ResetCauseClear();

  // Configure the Push Buttons and the LEDs 
  initGPIO();

  // Setup SysTick Timer for 1 msec interrupts  
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

  // Configure and Initialize the Watchdog timer 
  initWDOG();

  // Check if Power on Reset (POR) triggered the last reset 
  if (resetCause & RMU_RSTCAUSE_WDOGRST)
  {
    // Turn LED0 and LED1 ON 
    GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN); // LED logic is inverted
    while(1); //Stay here
  }

  // Enter loop, and feed the WDOG 
  while (1)
  {
	// Do not feed the WDOG if PB0 is pressed 
    while(!(GPIO_PinInGet(BSP_GPIO_PB0_PORT,BSP_GPIO_PB0_PIN)))
    {
      // Enter EM3 while the button is pressed
      // (WDOG will continue running)
      EMU_EnterEM3(true);
    }
  // Feed the watchdog 
	WDOG_Feed();

	// Toggle LED0 and LED1 at 10 kHz 
	GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
	Delay(100);
  }
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Enable clock for the GPIO module 
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure the GPIO pins F4 and F5 for the LEDs as output pins 
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  // Configure PB0 as input  
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);

}

/**************************************************************************//**
 * @brief Watchdog initialization
 *****************************************************************************/
void initWDOG(void)
{
  // Enabling clock to the interface of the low energy modules (including the Watchdog)
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Watchdog Initialize settings 
  WDOG_Init_TypeDef wdogInit = WDOG_INIT_DEFAULT;
  wdogInit.debugRun = true;
  wdogInit.em3Run = true;
  wdogInit.clkSel = wdogClkSelULFRCO;
  wdogInit.perSel = wdogPeriod_2k; // 2049 clock cycles of a 1kHz clock  ~2 seconds period

  // Initializing watchdog with chosen settings 
  WDOG_Init(&wdogInit);
}
