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
#include "em_gpio.h"
#include "em_chip.h"

#include "bspconfig.h"


// Global Variables 
unsigned long resetCause;
volatile uint32_t msTicks; // counts 1ms timeTicks 

// Function Declarations 
void initGPIO(void);
void startCRYO(void);

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
void GPIO_EVEN_IRQHandler(void)
{
  // Get and clear all pending GPIO interrupts
  GPIO_IntClear(GPIO_IntGet());

  // Configure EM4 for Cryotimer Wakeup.
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Shutoff;
  em4Init.pinRetentionMode = emuPinRetentionDisable;
  em4Init.retainUlfrco = true;
  EMU_EM4Init(&em4Init);

  // Initialize and start Cryotimer
  startCRYO();

  // Go into EM4 and wait for CRYOTIMER wake; LED(s) will be off
  EMU_EnterEM4();
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for odd pins
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
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

  // Init DCDC regulator with kit specific parameters 
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

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

      // Turn OFF LED 1 
      GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    }
    // Check if external reset (pin reset) triggered the last reset 
    else if (resetCause & RMU_RSTCAUSE_EXTRST)
    {
      // Turn OFF LED0 
      GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

      // Turn ON LED 1 
      GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    }
    // Check if entering/exiting EM4 triggered the last reset
    else if (resetCause & RMU_RSTCAUSE_EM4RST)
    {
      // Turn ON LED0
      GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

      // Turn ON LED 1
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
  // Enable clock for the GPIO module
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure the GPIO pins F4 and F5 for the LEDs as output pins 
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
 * @brief  Initialize and Start Cryotimer
 *****************************************************************************/
void startCRYO(void)
{
  // Init Cryotimer with specific parameters 
  CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;

  // Enable clock to CRYOTIMER module 
  CMU_ClockEnable(cmuClock_CRYOTIMER ,true);

  // Configure the CRYOTIMER to use the ULFRCO which is running at 1 KHz
  // and trigger an EM4 wakeup every 1024/1000 = 1,024s.
  init.debugRun = true;
  init.osc = cryotimerOscULFRCO;
  init.presc = cryotimerPresc_1;
  init.period = cryotimerPeriod_1k;
  init.em4Wakeup = true;

  CRYOTIMER_Init(&init);

  // Interrupt Setup
  CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);
  CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
  NVIC_ClearPendingIRQ(CRYOTIMER_IRQn);
  NVIC_EnableIRQ(CRYOTIMER_IRQn);

  // Enable the Cryotimer
  CRYOTIMER_Enable(true);
}

/**************************************************************************//**
 * @brief Cryotimer Interrupt handler
 *****************************************************************************/
void CRYOTIMER_IRQHandler(void)
{
  // Clear the CRYOTIMER interrupt
  CRYOTIMER_IntClear(CRYOTIMER_IFC_PERIOD);
  //
  // Flush instructions to make sure the interrupt is not re-triggered.
  // This is generally required when the peripheral clock is slower than
  // the CPU core clock.
  //
  __DSB();
}
