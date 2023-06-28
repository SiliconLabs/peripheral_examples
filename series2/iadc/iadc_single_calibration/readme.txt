iadc_single_calibration

This project demonstrates calibrating the IADC peripheral, followed by 
single polled differential analog measurements. The main program steps 
through the procedure outlined in Gain and Offset Correction section of the 
device Reference Manual for calibrating the onboard IADC. Once calibration is 
complete, the program resembles the single differential polled example, taking 
measurements and storing the IADC result and a voltage conversion of the result
into two global variables.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1.  Update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  Open the Simplicity Debugger and add "sample" and "singleResult" to the 
    Expressions Window
4.  Set a breakpoint at the first function within the infinite while loop 
    (IADC_command)
5.  Run the example
6.  Using a precision voltage source, supply full scale positive voltage across 
    differential positive and negative inputs (see below);
    This example uses AVDD (3.30V) as the reference voltage; 3.30V should be 
    applied to positive input and 0.0V should be applied to negative input
7.  Once the power source is connected, press and release PB0
8.  Remove the power source from differential inputs and jumper the two pins to 
    apply a zero volt potential across the IADC inputs
9.  Once the IADC inputs are shorted, press and release PB0 again
10. At the breakpoint, observe the raw data and measured voltage in the 
    Expressions Window. Measurement results should be near zero voltage as the 
    IADC inputs are still shorted
11. Remove the jumper from across the differential inputs, and connect a 
    variable voltage source (source should stay at or below full scale to avoid 
    damaging the IADC and MCU)
12. Vary the input voltage and resume the debugger to make a new conversions. 
    Observe how IADC result responds to different voltage values on the 
    corresponding pins (see below).

The first time the program halts at the breakpoint, the values in the 
Expression Window will be zero (default values). Each subsequent break will show
the previous conversion loop results within the Expressions Window.

================================================================================

Peripherals Used:
CMU     - FSRCO @ 20 MHz
GPIO
IADC    - 12-bit resolution, 
        - Automatic Two's Complement (differential = bipolar) 
        - Unbuffered 3.3V (AVDD) IADC voltage reference
        - IADC and reference kept in warmup mode
        - Conversions triggered by firmware
               
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PC04 -  IADC positive differential input, J102->Pin 6 of BRD4001, WSTK P25
PC05 -  IADC negative differential input, J102->Pin 8 of BRD4001, WSTK P27
PD02 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PC04 -  IADC positive differential input, J102->Pin 6 of BRD4001, WSTK P25
PC05 -  IADC negative differential input, J102->Pin 8 of BRD4001, WSTK P27
PB00 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PD04 -  IADC positive differential input, J102->Pin 6 of BRD4001, WSTK P25
PD05 -  IADC negative differential input, J102->Pin 5 of BRD4001, WSTK P24
PB01 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PD04 -  IADC positive differential input, J102->Pin 6 of BRD4001, WSTK P25
PD05 -  IADC negative differential input, J102->Pin 5 of BRD4001, WSTK P24
PB01 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06 -  GPIO Push/Pull output, WSTK P19, LED0
PD04 -  IADC positive differential input, WSTK P24
PD05 -  IADC negative differential input, WSTK P31
PB00 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB01 -  GPIO Push/Pull output, Expansion Header Pin 9, WSTK P6, LED1
PA05 -  IADC positive differential input, Expansion Header pin 12, WSTK P9
PA06 -  IADC negative differential input, Expansion Header pin 14, WSTK P11
PB00 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PD04 -  IADC positive differential input, WSTK P25
PD05 -  IADC negative differential input, WSTK P24
PB01 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK