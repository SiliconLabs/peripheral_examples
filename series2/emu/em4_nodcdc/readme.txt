em4_nodcdc

This project demonstrates the demonstrates datasheet current consumption 
configuration and current consumption numbers in EM4 Energy Mode.

Note: Default project gives EM4 current consumption numbers with no BURTC. 
For EM4 current consumption with BURTC running on LFXO, set BURTC_LFXO_EN to 1

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