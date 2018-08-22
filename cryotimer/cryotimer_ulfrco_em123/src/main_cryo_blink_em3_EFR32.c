/**************************************************************************//**
 * @file
 * @brief This code demonstrates use of the CRYOTIMER with the ULFRCO in EM3
 * (or EM1 or EM2).  The program flow is as follows:
 *
 * 1. Chip and DC-DC initialization.
 * 2. Configure LED0 pin as an output and drive high (turn on LED0).
 * 3. Configure CRYOTIMER for time out after 2K ULFRCO periods (around
 *    2 seconds) and enable interrupts.
 * 4. Go into EM3 and wait for CRYOTIMER interrupt.
 * 5. Clear flag in CRYOTIMER ISR and toggle LED0 toggles.
 *
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_usart.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_cryotimer.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "mx25flash_spi.h"

/**************************************************************************//**
 * @brief CRYOTIMER interrupt service routine
 *****************************************************************************/
void CRYOTIMER_IRQHandler(void)
{
	// Clear the CRYOTIMER interrupt
	CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);

	/*
	 * Flush instructions to make sure the interrupt is not re-triggered.
	 * This is generally required when the peripheral clock is slower than
	 * the CPU core clock.
	 */
	__DSB();

	// Toggle LED0 off or on
	GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/**************************************************************************//**
 * @brief Note: No need to enable the ULFRCO since it is always on and
 * cannot be shut off under software control.  The ULFRCO is used in this
 * example because it is the only oscillator running in EM3.
 *****************************************************************************/
void startCryo(void)
{
	CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;

	// Enable CRYOTIMER clock
	CMU_ClockEnable(cmuClock_CRYOTIMER, true);

	// Set CRYOTIMER parameters
	init.osc = cryotimerOscULFRCO;
	init.presc = cryotimerPresc_1;
	init.period = cryotimerPeriod_2k;	// nominally 2 seconds
	init.enable = true;

	// Interrupt setup
	CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);
	CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
	NVIC_ClearPendingIRQ(CRYOTIMER_IRQn);
	NVIC_EnableIRQ(CRYOTIMER_IRQn);

	CRYOTIMER_Init(&init);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
	CHIP_Init();

	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
	EMU_DCDCInit(&dcdcInit);

	// Put Serial Flash into Deep Power Down Mode
	MX25_init() ;
	MX25_DP() ;

	// Enable GPIO clock
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED0 pin as push pull output, initially high (LED0 on)
	GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);

	// Initialize and start CRYOTIMER
	startCryo();

	// Go into EM3 and let the CRYOTIMER IRQ handler take over
	while(1) EMU_EnterEM3(false);
}
