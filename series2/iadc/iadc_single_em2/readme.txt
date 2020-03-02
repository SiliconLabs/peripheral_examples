iadc_single_em2

This project demonstrates using the IADC peripheral to continuously take a single-ended
analog measurement while running in low power EM2. IADC interrupts on conversion completion
wake the MCU into EM0, where the IADC interrupt handler converts the result to
a voltage before going back into EM2. The IADC sampling rate is 1 ksps, and the 
ADC reads GPIO pin PC04 (J102 of BRD4001, Pin 25) as input. The IADC interrupt handler
also toggles a GPIO (EXP13, WSTK P10) with each conversion.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "sample" and "singleResult" to the 
Expressions Window
4. Observe GPIO output using an oscilloscope; toggles occur on every conversion, output
appears as 500HZ, 50% duty cycle clock
5. Suspend the debugger, observe the measured voltage change in the Expressions Window
and how it responds to different voltage values on the corresponding pins (see below)

Peripherals Used:
CLK_CMU_ADC  - 1 MHz HFRCOEM23 for EFR32xG21;  20 MHz FSRCO for EFR32xG22
CLK_SRC_ADC  - 1 MHz for EFR32xG21;            5 MHz for EFR32xG22
CLK_ADC      - 10 kHz for EFR32xG21;           10 kHz for EFR32xG22
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in warmup mode
			 - Conversions initiated by firmware and triggered continuously
			   (when a conversion completes a new one is requested immediately without 
			   requiring a new trigger)

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25