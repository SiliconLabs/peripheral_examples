iadc_single_calibration

This project demonstrates calibrating the IADC peripheral, followed by 
single polled differential analog measurements. The main program steps 
through the proceedure outlined in Gain and Offset Correction section of the device 
Reference Manual for calibrating the onboard IADC. Once calibration is complete,
the program resembles the single differential polled example, taking measurements
and storing the IADC result and a voltage conversion of the result into two 
global variables.

How To Test:
1.  Update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  Open the Simplicity Debugger and add "sample" and "singleResult" to the Expressions Window
4.  Set a breakpoint at the first function within the infinite while loop (IADC_command on line 257)
5.  Run the example
6.  Using a precision voltage source, supply full scale positive voltage across PC04 and PC05;
    This example uses AVDD (3.30V) as the reference voltage; 3.30V should be applied to PC04 and 
	0.0V should be applied to PC05
7.  Once the power source is connected, press and release PB0
8.  Remove the power source from PC04 and PC05 and jumper the two pins to apply a zero volt potential
    across the IADC inputs
9.  Once the IADC inputs are shorted, press and release PB0 again
10. At the breakpoint, observe the raw data and measured voltage in the Expressions Window.
    Measurement results should be near zero voltage as the IADC inputs are still shorted
11. Remove the jumper from across PC04 and PC05, and connect a variable voltage source (source 
    should stay at or below full scale to avoid damaging the IADC and MCU)
12. Vary the input voltage and resume the debugger to make a new conversions. Observe how IADC result
    responds to different voltage values on the corresponding pins (see below).

The first time the program halts at the breakpoint, the values in the Expression Window
will be zero (default values). Each subsequent break will show the previous conversion
loop results within the Expressions Window.

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
PB00 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 -  IADC positive differential input, J102 of BRD4001, Pin 25
PC05 -  IADC negative differential input, J102 of BRD4001, Pin 27
PD02 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 -  IADC input, single-ended, J102 of BRD4001, Pin 25
PC05 -  IADC input, single-ended, J102 of BRD4001, Pin 27
PB00 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK