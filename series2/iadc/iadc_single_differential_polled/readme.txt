iadc_single_differential_polled

This project demonstrates using the IADC peripheral to take a differential
analog measurement. The main program polls continuously for conversions,
then stores the IADC raw result and a voltage conversion of the result into two 
global variables.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "singleResult" to the Expressions Window
4. Set a breakpoint at the first function within the infinite while loop 
   (IADC_command)
5. Run the example project
6. At the breakpoint, observe the measured voltage in the Expressions Window and
   how it responds to different voltage values on the corresponding pins (see 
   below).

The first time the program halts at the breakpoint, the values in the Expression
Window will be zero (default values). Each subsequent break will show the 
previous conversion loop results within the Expressions Window.

Note: If you want to view the raw result from the IADC instead of the converted 
voltage, you can watch "sample" in addition to the "singleResult" in the 
Expressions window.

================================================================================

Peripherals Used:
CMU     - FSRCO @ 20 MHz
GPIO
IADC    - 12-bit resolution, 
        - Automatic Two's Complement (differential = bipolar) 
        - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
        - IADC and reference kept in warmup mode
        - Conversions triggered by firmware
               
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  IADC positive differential input, Expansion Header Pin 11, WSTK P8
PB01 -  IADC negative differential input, Expansion Header Pin 13, WSTK P10

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB00 -  IADC positive differential input, Expansion Header Pin 7, WSTK P4
PB01 -  IADC negative differential input, Expansion Header Pin 9, WSTK P6

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB00 -  IADC positive differential input, WSTK P15
PB01 -  IADC negative differential input, WSTK P17

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB00 -  IADC positive differential input, WSTK P15
PB01 -  IADC negative differential input, WSTK P17

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  IADC positive differential input, WSTK P17
PB01 -  IADC negative differential input, WSTK P21

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 -  IADC positive differential input, Expansion Header Pin 7, WSTK P4
PB01 -  IADC negative differential input, Expansion Header Pin 9, WSTK P6

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB00 -  IADC positive differential input, WSTK P15
PB01 -  IADC negative differential input, WSTK P17