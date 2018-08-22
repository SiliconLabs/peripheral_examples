/**************************************************************************//**
 * @file
 * @brief This code demonstrates use of the CRYOTIMER in EM3 and EM4 with no
 * GPIO retention.  The program flow is as follows:
 *
 * 1. Chip and DC-DC initialization.
 * 2. Determine and clear reset cause(s):
 *    a. If a primary source like POR, the reset pin, or system reset
 *       request (debugger), turn off LED1.
 *    b. If the reset is due to EM4 wake-up, turn on LED1.
 * 3. Turn on LED0.
 * 4. Prepare for EM4 shutdown entry with ULFRCO active and no GPIO
 *    retention (GPIOs immediately reset).
 * 5. Configure CRYOTIMER for time out after 2K ULFRCO periods (around
 *    2 seconds), wake from EM4, and enable interrupts.
 * 6. Go into EM3 and wait for CRYOTIMER interrupt.
 * 7. Go into EM4 and wait for CRYOTIMER wake-up.  GPIOs assume reset
 *    state (LEDs turn off).
 * 8. Go back to step 1 upon EM4 wake-up.
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
#include "em_chip.h"
#include "em_cmu.h"
#include "em_cryotimer.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_rmu.h"
#include "bsp.h"


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
}

/**************************************************************************//**
 * @brief Note: No need to enable the ULFRCO since it is always on and
 * cannot be shut off under software control.
 *****************************************************************************/
void startCryo(void)
{
	CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;

	// Enable CRYOTIMER clock
	CMU_ClockEnable(cmuClock_CRYOTIMER, true);

	// Clear CRYOTIMER_IF PERIOD flag; it will be set upon EM4 wake
	CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);

	/*
	 * Set CRYOTIMER parameters.  Note that disabling the CRYOTIMER is
	 * necessary after EM4 wake in order to reset the counter, otherwise
	 * the next delay before wake won't be the full 2K periods.
	 */
	init.enable = false;
	init.em4Wakeup = true;
	init.osc = cryotimerOscULFRCO;
	init.presc = cryotimerPresc_1;
	init.period = cryotimerPeriod_2k;	// nominally 2 seconds

	CRYOTIMER_Init(&init);

	// Interrupt setup
	CRYOTIMER_IntClear(CRYOTIMER_IF_PERIOD);
	CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
	NVIC_ClearPendingIRQ(CRYOTIMER_IRQn);
	NVIC_EnableIRQ(CRYOTIMER_IRQn);

	CRYOTIMER_Enable(true);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
	CHIP_Init();

	EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
	EMU_DCDCInit(&dcdcInit);

	// Enable GPIO clock
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Get reset cause(s)
	uint32_t rflags = RMU_ResetCauseGet();

	/*
	 * Did we get reset from power-on, external pin, or debugger (system
	 * request)?  If so, configure PF5 as an output and turn LED1 off.
	 * Otherwise turn LED1 on if we got EM4 wake.
	 */
	if ((rflags & RMU_RSTCAUSE_PORST) ||
			(rflags & RMU_RSTCAUSE_EXTRST) ||
			(rflags & RMU_RSTCAUSE_SYSREQRST))
		GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
	else if (rflags & RMU_RSTCAUSE_EM4RST)
		GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1);

	// Configure LED0 pin as push pull output, initially high (LED0 on)
	GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);

	// Clear reset causes so we know which reset matters next time around
	RMU_ResetCauseClear();

	// Configure EM4 behavior
	EMU_EM4Init_TypeDef init = EMU_EM4INIT_DEFAULT;

	init.em4State = emuEM4Shutoff;	// emuEM4Hibernate also works
	init.retainUlfrco = true;
	init.pinRetentionMode = emuPinRetentionDisable;
	EMU_EM4Init(&init);

	// Initialize and start CRYOTIMER
	startCryo();

	// Go into EM3 and wait for CRYOTIMER IRQ; LED(s) will be on
	EMU_EnterEM3(false);

	// Go into EM4 and wait for CRYOTIMER wake; LED(s) will be off
	EMU_EnterEM4();
}
