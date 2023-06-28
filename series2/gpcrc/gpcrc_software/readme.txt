gpcrc_software

This project demonstrates the GPCRC used to check an array of 32-bit data
using the IEEE 802.3 polynomial standard. The GPCRC conversions are initiated
by software.

Functionality is included to show how one could perform this conversion 
without the GPCRC.  Of note, the pure software method requires more memory and
can take 3-5 times longer to compute for single words, and the gap grows the more
consecutive transfers you do.

Note for EFR32MG21, clock enabling is not required.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "results" to the Expressions window
4. Run the debugger, then pause it.  You should notice that "results" is
   filled with "checked" values

================================================================================

Peripherals Used:
HFRCO   - 19 MHz
GPCRC   - IEEE 802.3 poly standard

================================================================================

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68