iadc_scan_timer

This project demonstrates using the IADC peripheral to take single-ended
analog measurements of two input channels. IADC interrupts handle 
completed conversions and store them in a global array. The IADC conversion
is triggered using the IADC internal timer, which is configured to occur every
1ms. WSTK LED1 is also toggled on each conversion.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "scanResult" to the Expressions Window
4. Observe GPIO output using an oscilloscope; toggles occur on every conversion (every 1ms)
output appears as 500HZ, 50% duty cycle clock
5. Suspend the debugger, observe the measured voltages in the Expressions Window
and how they respond to different voltage values on the corresponding pins (see below)

Peripherals Used:
CLK_CMU_ADC  - 80 MHz HFRCODPLL for Series 2
CLK_SRC_ADC  - 40 MHz for Series 2
CLK_ADC      - 10 MHz for Series 2 
timerCycles  - 40000 CLK_SRC_ADC cycles -> 1ms Timer
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in warmup mode
			 - Conversions triggered by local IADC timer

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25
PC05 - IADC input, single-ended, J102 of BRD4001, Pin 27

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25
PC05 - IADC input, single-ended, J102 of BRD4001, Pin 27