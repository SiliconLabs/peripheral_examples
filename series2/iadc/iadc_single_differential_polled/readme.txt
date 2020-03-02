iadc_single_differential_polled

This project demonstrates using the IADC peripheral to take a differential
analog measurement. The main program polls continuously for conversions,
then stores the IADC result and a voltage conversion of the result into two 
global variables.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "singleResult" to the Expressions Window
4. Set a breakpoint at the first function within the infinite while loop (IADC_command on line 115)
5. Run the example project
6. At the breakpoint, observe the measured voltage in the Expressions Window
and how it responds to different voltage values on the corresponding pins (see below).

The first time the program halts at the breakpoint, the values in the Expression Window
will be zero (default values). Each subsequent break will show the previous conversion
loop results within the Expressions Window.

Note: If you want to view the raw result from the IADC instead of the converted voltage,
you can watch "sample" in addition to the "singleResult" in the Expressions window.

Peripherals Used:
CLK_CMU_ADC  - 20 MHz FSRCO clock for Series 2
CLK_SRC_ADC  - 10 MHz for Series 2
CLK_ADC      - 10 MHz for Series 2
IADC         - 12-bit resolution, Automatic Two's Complement (differential = bipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in warmup mode
			 - Conversions triggered by firmware
               
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PC04 -  IADC positive differential input, J102 of BRD4001, Pin 25
PC05 -  IADC negative differential input, J102 of BRD4001, Pin 27

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PC04 -  IADC input, single-ended, J102 of BRD4001, Pin 25
PC05 -  IADC input, single-ended, J102 of BRD4001, Pin 27