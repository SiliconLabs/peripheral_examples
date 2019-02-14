/**************************************************************************//**
 * @file
 * @brief This project demonstrates the ability for a pin to wake the device
 * from EM4. See Readme.txt for more information.
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
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "em_rmu.h"
#include "bsp.h"

#define RMU_EXTRST_XMASK 		  0x00000009UL
#define RMU_BODREGRST_XMASK     0x0000001FUL
#define RMU_BODUNREGRST_XMASK   0x00000003UL


/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void) 
{
  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD15 as an output
  GPIO_PinModeSet(gpioPortD, 15, gpioModePushPull, 0);

  // Configure LED0 and LED1 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief Toggle STK LEDs forever
 *****************************************************************************/
void toggleLEDs(void) 
{
  while (1) {
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
    GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);

    // Arbitrary delay between toggles
    for (volatile uint32_t delay = 0; delay < 0xFFFFF; delay++);
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void) 
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initGPIO();

  // Get the last Reset Cause
  uint32_t rstCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  // Set DISP_ENABLE(PD15) low to clear LCD
  GPIO_PinOutClear(gpioPortD, 15);

  // If the last Reset was due to leaving EM4 via external reset, toggle LEDs. Else, delay and enter EM4
  if (((rstCause & RMU_EXTRST_XMASK) == 0x8) | ((rstCause & RMU_BODREGRST_XMASK) == 0x4) | ((rstCause & RMU_BODUNREGRST_XMASK) == 0x2))
  {
    toggleLEDs();
  }
  else
  {
    for (volatile uint32_t delay = 0; delay < 0xFFF; delay++);
    EMU_EnterEM4();
  }
  
  // Will never get here!
  while (1);
}
