iadc_single_16bit

This project demonstrates using the IADC peripheral's oversampling feature to
acquire a 16-bit resolution conversion result while operating in EM2.
IADC interrupts on conversion completion wake the MCU into EM0, where the IADC
interrupt handler converts the result to a voltage before going back into EM2.
The IADC sampling rate is 77 ksps with an oversampling rate of 32, and the 
ADC reads GPIO pin PC04 (J102 of BRD4001, Pin 25) as input. The PRS peripheral
will output a pulse on PB1 whenever the IADC finishes one single conversion.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary).
2. Build the project and download to the Starter Kit.
3. Open the Simplicity Debugger and add "sample" and "singleResult" to the 
   Expressions Window.
4. Apply a voltage to the ADC input pin (PC04).
5. Observe the sample field as it will display as a 16 bit result.
   The "singleResult" is obtained by the formula: sample*VREF/(2^16).
6. Observe PB1 toggling with an interval of 13us, 50 percent duty cycle.
   This corresponds to roughly 77KHz of sampling frequency.
7. Suspend the debugger, observe the measured voltage change in the Expressions
   Window and how it responds to different voltage values on the corresponding
   pins (see below).

Peripherals Used:
CLK_CMU_ADC  - 20 MHz FSRCO for EFR32xG22
CLK_SRC_ADC  - 20 MHz for EFR32xG22
CLK_ADC      - 10 MHz for EFR32xG22
IADC         - 16-bit resolution, Oversampling 32X
               Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in warmup mode
			 - Conversions initiated by firmware and triggered continuously
			   (when a conversion completes a new one is requested immediately
			   without requiring a new trigger)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB01 - GPIO Push/Pull output, Expansion Header Pin 9, WSTK Pin 6
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25
