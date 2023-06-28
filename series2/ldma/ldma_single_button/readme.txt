ldma_single_button

This example is based on the EFR32 Series 2 Reference Manual:
Chapter: LDMA
Section: Examples
Subsection: Example #1

In this example, the LDMA transfers 128 halfwords (256 bytes) from one software
array to another. The transfer is requested by software (#define USE_GPIO_PRS 0)
or a button press on Push Button 1 (#define USE_GPIO_PRS 1).

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4. Add a breakpoint at the beginning of LDMA_IRQHandler()
5. The transfer is requested by software (#define USE_GPIO_PRS 0):
   Run the debugger. It should halt inside the interrupt subroutine with the
   transfer complete (this can be seen in the Expressions window)
6. The transfer is requested by a button press on PB1 (#define USE_GPIO_PRS 1):
   Run the debugger. After you press the PB1, the debugger should halt inside
   the interrupt subroutine with the transfer complete (this can be seen in the
   Expressions window)

================================================================================

Peripherals Used:
HFRCODPLL - 19 MHz
LDMA      - Channel 0
PRS       - Channel 1, Push Button PB1
GPIO

================================================================================

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21A010F1024IM32
PD03 - Push Button PB1

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB01  - push button PB1

Board:  Silicon Labs EFR32ZG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB03 - push button PB1, WSTK Pin 21

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB03 - push button PB1, WSTK Pin 21

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB01 - Push Button PB1, WSTK Pin 21

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB01 - Push Button PB1, WSTK Pin 6, Expansion Header Pin 9

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB03 - Push Button PB1, WSTK Pin 21