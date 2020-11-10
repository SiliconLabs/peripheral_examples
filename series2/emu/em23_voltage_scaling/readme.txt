em23_voltage_scaling

This project demonstrates the use of EM2/EM3 voltage scaling on Series 2
EFR32 devices.  Note that this differs from the voltage scaling
functionality on Series 1 devices where a single setting (VS2 or VS0)
applies in all operating modes.  For Series 2 devices, separate examples
are provided for EM0/1 and EM2/3 voltage scaling.

In this example, the CPU configures the BURTC to generate interrupts at
4-second intervals, disables the lower 24 KB of RAM, and then enters EM3
with VS2 voltage scaling.  Upon wake-up, the EM23 voltage scaling is
switched to VS0, and the device re-enters EM3.  This toggling between VS2
and VS0 occurs every four seconds so that it can observed in Simplicity
Studio's Energy Profiler or by powering the WSTK from a bench supply with
a digital multimeter in series and sliding the 3-position BAT/USB/AEM
switch from the AEM to the BAT position.

The savings when running at VS2 (1.1V) vs. VS0 (0.9V) in EM3 with the
ULFRCO providing the clock for the BURTC is on the order of 40% (3.2 µA
vs. 1.7 µA) when observed using a standalone bench supply and digital
multimeter.  In Energy Profiler, the difference appears to be smaller
(6.6 µA) vs. (5.4 µA).

Note that when EFR32xG22 enters EM2/3, the connection to the debugger
is lost, which complicates regaining control of the device.  This
examples includes an "escape hatch" mechanism to pause the device so
that a debugger can connect in order to erase flash.  To use this, press
the PB0 push button and, while holding the button, press the RESET button
in the lower right corner of the WSTK.  Upon releasing the RESET button,
the device runs code that sees that PB0 is depressed, turns on LED0, and
issues a breakpoint instruction to halt the CPU.  At this point, a
debugger can be connected to erase flash, etc.

================================================================================

Peripherals Used:

BURTC
GPIO
CMU

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Wireless Starter Kit, then exit
   from the debugger.
3. Open the Simplicity Studio's Energy Profiler.  Select Start Energy Capture
   from the Profiler menu.
4. Zoom in on the Y-axis (current) and observe the change in current draw
   as the devices toggles between VS2 and VS0 every four seconds.

================================================================================

This is example is intended to run on the following setup(s):

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG22C224F512 (this code will run unchanged on the radio
board for any EFR32xG22 variant).