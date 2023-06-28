ldma_interchannel_synchronization

This example is based on the EFR32 Series 2 Reference Manual:
Chapter: LDMA
Section: Examples
Subsection: Example #5

In this example, the LDMA synchronizes across 2 Channels. Each channel starts
off on a different button press, and transfers 4-character strings from one 
software array to another.

If you press PB0, "AAaa" will be transfered to dstBuffer.
If you press PB1, "YYyy" will be transfered to dstBuffer.
After you press the second button (regardless of the order), 
"CCcc" will be transferred to dstBuffer.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4. Run the debugger.  
5. Press either PB0 or PB1, and observe the value in dstBuffer before and after
   each button press change according to the behavior described above.
Note: You will have to pause the debugger to see the value in dstBuffer.

================================================================================

Peripherals Used:
HFRCODPLL - 19 MHz
LDMA      - Channel 0
PRS       - Channel 1, Push Button PB0
          - Channel 2, Push Button PB1
GPIO

================================================================================

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21A010F1024IM32
PD02 -  Push Button PB0
PD03 -  Push Button PB1

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB00 -  Push Button PB0
PB01 -  Push Button PB1

Board:  Silicon Labs EFR32ZG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  Push Button PB0, WSTK Pin 17
PB03 -  Push Button PB1, WSTK Pin 21

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  Push Button PB0, WSTK Pin 17
PB03 -  Push Button PB1, WSTK Pin 21

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  Push Button PB0, WSTK Pin 17
PB01 -  Push Button PB1, WSTK Pin 21

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 -  Push Button PB0, WSTK Pin 4, Expansion Header Pin 7
PB01 -  Push Button PB1, WSTK Pin 6, Expansion Header Pin 9

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  Push Button PB0, WSTK Pin 17
PB03 -  Push Button PB1, WSTK Pin 21