/**************************************************************************//**
 * @main.c
 * @brief This project demonstrates the ability for a pin to wake the device
 * from EM4.
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2019 Silicon Labs, Inc. http://www.silabs.com</b>
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

#define EM4WU_PIN           BSP_GPIO_PB0_PIN
#define EM4WU_PORT          BSP_GPIO_PB0_PORT
#define EM4WU_EM4WUEN_NUM   (9)                       // PD2 is EM4WUEN pin 9
#define EM4WU_EM4WUEN_MASK  (1 << EM4WU_EM4WUEN_NUM)

/**************************************************************************//**
 * @brief  Initialize GPIOs for push button and LED
 *****************************************************************************/
void initGPIO(void)
{
  // Configure Button PB0 as input and EM4 wake-up source
  GPIO_PinModeSet(EM4WU_PORT, EM4WU_PIN, gpioModeInputPullFilter, 1);
  GPIO_EM4EnablePinWakeup(EM4WU_EM4WUEN_MASK << _GPIO_EM4WUEN_EM4WUEN_SHIFT, 0);

  // Configure LED0 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief Toggle LEDs on STK and Radio board indefinitely
 *****************************************************************************/
void toggleLEDs(void)
{
  while(1)
  {
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

    // Arbitrary delay between toggles
	for(volatile uint32_t delay = 0; delay < 0xFFFFF; delay++);
  }
}

/**************************************************************************//**
 * @brief	Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialization
  initGPIO();
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  EMU_EM4Init(&em4Init);

  // Get the last Reset Cause
  uint32_t rstCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  // If the last Reset was due to leaving EM4, toggle LEDs. Else, enter EM4
  if(rstCause & EMU_RSTCAUSE_EM4)
  {
    toggleLEDs();
  }
  else
  {
	EMU_EnterEM4();
  }

  // This line should never be reached
  while(1);
}
