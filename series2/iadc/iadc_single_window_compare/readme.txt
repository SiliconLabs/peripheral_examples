iadc_single_window_compare

This project demonstrates using the IADC peripheral as a window comparator on a
single-ended input PC04 (J102 of BRD4001, Pin 25). The window comparator is configured
to trigger on output results which are inside the specified window. WSTK LED0
is toggled on each conversion with results within the specified window. The most recent
sample within the window comparison is also stored globally.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "sample" and "singleResult" to the 
Expressions window
4. Observe GPIO output (WSTK LED0) using an oscilloscope while varying the input voltage;
GPIO will toggle while the input is within the specified window, and hold steady to
the last state when the input voltage moves outside the window. 
5. Suspend the debugger, observe the measured voltage in the Expressions Window of the
most recent conversion within the specified window.

Peripherals Used:
CLK_CMU_ADC  - 38.4 MHz HFXO for Series 2
CLK_SRC_ADC  - 9.6 MHz for Series 2
CLK_ADC      - 100 kHz for Series 2
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in warmup mode 
			 - Conversions initiated by firmware and triggered continuously
			   (when a conversion completes a new one is requested immediately without 
			   requiring a new trigger)
			   
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 -  IADC input, single-ended, J102 of BRD4001, Pin 25

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 -  IADC input, single-ended, J102 of BRD4001, Pin 25