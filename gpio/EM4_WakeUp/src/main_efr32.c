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
#include "mx25flash_spi.h"
#include "em_usart.h"

#define EM4WU_PIN           BSP_GPIO_PB1_PIN
#define EM4WU_PORT          BSP_GPIO_PB1_PORT
#define EM4WU_EM4WUEN_NUM   (1)                       // PB1 is EM4WUEN pin 1
#define EM4WU_EM4WUEN_MASK  (1 << EM4WU_EM4WUEN_NUM)

#define EM4_RSTCAUSE_MASK	RMU_RSTCAUSE_EM4RST

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void) 
{
  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB1 as input and EM4 wake-up source
  GPIO_PinModeSet(EM4WU_PORT, EM4WU_PIN, gpioModeInputPullFilter, 1);
  GPIO_EM4EnablePinWakeup(EM4WU_EM4WUEN_MASK << _GPIO_EM4WUEN_EM4WUEN_SHIFT, 0);

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

  // Put Serial Flash into Deep Power Down Mode
  MX25_init() ;
  MX25_DP() ;

  // If the last Reset was due to leaving EM4, toggle LEDs. Else, enter EM4
  if (rstCause == EM4_RSTCAUSE_MASK)
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
