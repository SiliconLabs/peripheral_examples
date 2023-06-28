iadc_single_window_compare

This project demonstrates using the IADC peripheral as a window comparator on a
single-ended input PC05. The window comparator is configured to trigger on 
output results which are inside the specified window. WSTK LED0 is toggled on 
each conversion with results within the specified window. The most recent
sample within the window comparison is also stored globally.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "sample" and "singleResult" to the 
   Expressions window
4. Observe GPIO output (WSTK LED0) using an oscilloscope while varying the input
   voltage; GPIO will toggle while the input is within the specified window, and
   hold steady to the last state when the input voltage moves outside the window 
5. Suspend the debugger, observe the measured voltage in the Expressions Window
   of the most recent conversion within the specified window.

================================================================================

Peripherals Used:
CMU     - HFXO
GPIO
IADC    - 12-bit resolution, Automatic 2's Complement (single-ended = unipolar) 
        - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
        - IADC and reference kept in warmup mode 
        - Conversions initiated by firmware and triggered continuously
          (when a conversion completes a new one is requested immediately 
          without requiring a new trigger)
			   
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PC05 -  IADC input, single-ended, J102->Pin 8 of BRD4001, WSTK P27

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PC05 -  IADC input, single-ended, J102->Pin 8 of BRD4001, WSTK P27

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PC05 -  IADC input, single-ended, Expansion Header Pin 15, WSTK P12

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PC05 -  IADC input, single-ended, Expansion Header Pin 15, WSTK P12

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06 -  GPIO Push/Pull output, WSTK P27, LED0
PC05 -  IADC input, single-ended, WSTK P33

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00  - GPIO Push/Pull output, J101->Pin 9 of BRD4001, WSTK P4, LED0
PC05 -  IADC input, single-ended, J102->Pin 8 of BRD4001, WSTK P27

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PC05 -  IADC input, single-ended, Expansion Header Pin 15, WSTK P12