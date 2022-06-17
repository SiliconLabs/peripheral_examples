em1_dcdc

This project demonstrates the datasheet current consumption 
configuration and current consumption numbers in EM1 Energy Mode.

Note: Default project gives EM1 current consumption numbers when DCDC is enabled 
with all peripherals disabled and HFRCO running at 38 MHz. 
To get EM1 energy consumption at different HFRCO frequencies change HFCRO DPLL 
band in line 73 of main.c.

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
CMU    - HFRCODPLL @ 38 MHz

Board: Silicon Labs EFR32FG23 868-915 MHz 14 dBm Board (BRD4263B)
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG23A010F512

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56