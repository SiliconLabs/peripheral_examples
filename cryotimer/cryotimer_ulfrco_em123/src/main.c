/**************************************************************************//**
 * @main.c
 * @brief This project shows how to use the Cryotimer with the ULFRCO in EM3.
 * The project idles in EM3 while waiting for the interrupt handler to toggle
 * LED0.
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
#include "em_emu.h"
#include "em_gpio.h"
#include "em_cryotimer.h"
#include "bsp.h"

// Note: change this to one of the defined periods in em_cryotimer.h
// Wakeup events occur every 2048 prescaled clock cycles
#define CRYOTIMER_PERIOD    cryotimerPeriod_2k

// Note: change this to one of the defined prescalers in em_cryotimer.h
// The clock is divided by one
#define CRYOTIMER_PRESCALE  cryotimerPresc_1

/**************************************************************************//**
 * @brief
 *    Cryotimer interrupt service routine
 *
 * @note
 *    The barrier below is make sure the interrupt flags get cleared before
 *    continuing. This ensures that the interrupt is not retriggered before
 *    exiting the Handler. A barrier like this is generally required when the
 *    peripheral clock is much slower than the CPU core clock.
 *
 * @note
 *    Despite the note above, the barrier is just a precaution. The code
 *    might work without it as well.
 *****************************************************************************/
void CRYOTIMER_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = CRYOTIMER_IntGet();
  CRYOTIMER_IntClear(flags);

  // Put a barrier here to ensure interrupts are not retriggered. See note above
  __DSB();

  // Toggle LED0
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/**************************************************************************//**
 * @brief
 *    Initialize Cryotimer
 *
 * @note
 *    No need to enable the ULFRCO since it is always on and cannot be shut off
 *    under software control. The ULFRCO is used in this example because it is
 *    the only oscillator capable of running in EM3.
 *****************************************************************************/
void initCryotimer(void)
{
  // Enable cryotimer clock
  CMU_ClockEnable(cmuClock_CRYOTIMER, true);

  // Initialize cryotimer
  CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;
  init.osc = cryotimerOscULFRCO;   // Use the ULFRCO
  init.presc = CRYOTIMER_PRESCALE; // Set the prescaler
  init.period = CRYOTIMER_PERIOD;  // Set when wakeup events occur
  init.enable = true;              // Start the cryotimer after initialization is done
  CRYOTIMER_Init(&init);

  // Enable cryotimer interrupts
  CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
  NVIC_EnableIRQ(CRYOTIMER_IRQn);
}

/**************************************************************************//**
 * @brief
 *    Initialize LED0 GPIO pin
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LED0 pin as output, initially high (LED0 on)
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initGpio();
  initCryotimer();

  // Go into EM3 and only wake up when the cryotimer interrupt occurs
  while(1) {
    EMU_EnterEM3(false);
  }
}
