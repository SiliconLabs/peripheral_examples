msc_rw

This project demonstrates the use of the userdata page of the
flash memory. The value 32 is stored in the 4th word of the page. A
variable is set to the value stored at this location, which can then
be read in the debugger.

Note: On EFR32xG21 devices, oscillators and clock branches are automatically 
turned on/off based on demand from the peripherals.  As such, writes to clock 
enable bits are unnecessary and have no effect on xG21 devices. 

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Run in the debugger 
3. Confirm that the Cleared_value is 4294967295 (Hex 0xFFFFFFFF)
4. Confirm that the Set_value is 32

================================================================================

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz
MSC
SE (xG21 only)

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56

Board:  Silicon Labs EFR32xG27 Buck Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68