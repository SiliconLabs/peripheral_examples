em1_dcdc

This project demonstrates the demonstrates datasheet current consumption 
configuration and current consumption numbers in EM1 Energy Mode.

Note: Default project gives EM1 current consumption numbers when DCDC is enabled 
with all peripherals disabled and HFRCO running at 38MHz.
To get EM1 energy consumption at different HFRCO frequencies change HFCRO DPLL 
band in line 76 of main.c.

================================================================================

Peripherals Used:
CMU

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Wireless Starter Kit, then exit
   from the debugger.
3. Open the Simplicity Studio's Energy Profiler.  Select Start Energy Capture
   from the Profiler menu and restart the wireless starter kit using Reset Pin.
4. Zoom in on the Y-axis (current) and observe the change in current draw.

================================================================================

This is example is intended to run on the following setup(s):

Board: Silicon Labs EFR32FG23 868-915 MHz 14 dBm Board (BRD4263B)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32FG23A010F512 (this code will run unchanged on the radio
board for any EFR32xG23 variant).