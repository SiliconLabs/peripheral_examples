em3_dcdc

This project demonstrates the datasheet current consumption 
configuration and current consumption numbers in EM3 Energy Mode.

Note: Default project gives EM3 current consumption numbers when DCDC is enabled
(VSCALE0) with RAM retention on block 0 only(power down all RAM except block 0).
For EM3 current consumption with full RAM retention, set POWER_DOWN_RAM to 0 and
modify RAM LENGTH in the linker script.

When EFR32xG22 and later devices enter EM2/3, the clock to the CPU debug
block is stopped, effectively dropping the host OC debugger connection.
This examples includes an "escape hatch" mechanism to pause the device so
that a debugger can connect in order to erase flash.  To use this, press
the PB0 push button and, while holding the button, press the RESET button
in the lower right corner of the WSTK.  Upon releasing the RESET button,
the device runs code that sees that PB0 is depressed, turns on LED0, and
issues a breakpoint instruction to halt the CPU.  At this point, a
the debugger connection can be resumed to erase flash, etc.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Wireless Starter Kit, then exit
   from the debugger.
3. Open the Simplicity Studio's Energy Profiler.  Select Start Energy Capture
   from the Profiler menu and restart the wireless starter kit using Reset Pin.
4. Zoom in on the Y-axis (current) and observe the change in current draw.

================================================================================

Note for Testing on xG25:
Simplicity Studio's Energy Profiler tool only measures the VMCU current. VMCU 
only powers IOVDD0-1 and the serial flash on BRD4270B. The 3.6 V LDO powers the
rest of the rails on the radio board. In the datasheet, the current 
consumption with DCDC test conditions requires IOVDD0-2, RFVDD, and DVDD to be 
powered by the output of the DCDC. Keep in mind, due to the design of the radio
board (BRD4270B), this example does not replicate the datasheet test conditions
followed for current consumption with DCDC. Although RFVDD and DVDD are powered 
by the output of the DCDC, IOVDD0-1 is powered by VMCU on the radio board and 
IOVDD2 is powered by the USB_VREG. To measure the 3.6V LDO current, the 
following extra steps must be taken.
1. Complete Steps 1 and 2 from the "How to Test" section above.
2. Select the device in the Debug Adapters pane within the Simplicity Studio
   Launcher view. 
2. Select the Documentation tab, then check the Schematic and Layout file 
   resource checkbox. Open the schematic and assembly files for BRD4270B.
3. Resistor R247 is a 0 Ohm resistor that can be removed to  measure the 
   current consumption of the device. Search for this resistor in the assembly 
   file to determine its position on the board. This resistor is present on the 
   underside of BRD4270B. 
4. Remove R247 and solder two leads to each pad where R247 was previously. 
   Connect these leads to a multimeter to measure the current consumption of the 
   device. 
   
================================================================================

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz, LFRCO @ 32.768 kHz
BURTC  - 32.768 kHz LFRCO clock source
USART0 - 8 Mbits/s
EUSART - 8 Mbits/s (xG25)

Board: Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Board (BRD4204D)
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PB01  - push button PB0
PB02  - LED0
PC01  - USART0 MOSI
PC02  - USART0 MISO
PC03  - USART0 SCLK
PC04  - USART0 CS

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01  - push button PB0
PB02  - LED0
PC01  - USART0 MOSI
PC02  - USART0 MISO
PC03  - USART0 SCLK
PC04  - USART0 CS

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00  - push button PB0
PC06  - LED0
PC00  - FLASH MOSI
PC01  - FLASH MISO
PC02  - FLASH SCLK
PC03  - FLASH CS