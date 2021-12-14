em01_voltage_scaling

This project demonstrates the use of EM0/EM1 voltage scaling on Series 2
EFR32 devices.  Note that this differs from the voltage scaling
functionality on Series 1 devices where a single setting (VS2 or VS0)
applies in all operating modes.  For Series 2 devices, separate examples
are provided for EM0/1 and EM2/3 voltage scaling.

In this example, the CPU recursively calculates Fibonacci numbers running
from RAM in order to make the difference in current draw more obvious.
Press the PB0 button on the Wireless Starter Kit (WSTK) mainboard to
switch between the two voltage scaling levels.  The difference in current
draw can be observed in Simplicity Studio's Energy Profiler or by powering
the WSTK from a bench supply with a digital multimeter in series and
sliding the 3-position BAT/USB/AEM switch from the AEM to the BAT position.

The savings when running at 38.4 MHz in VS2 (1.1V) vs. VS1 (1.0V) -- both
with zero flash wait states and just below the maximum operating frequency
limit of 40 MHz for VS1 -- is about 6%.  Real applications would keep
multiple peripherals and, therefore, their clocks enabled and would see a
savings in core current draw approaching the 9% difference between 1.1V
and 1.0V core operating voltage levels.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Wireless Starter Kit, then exit
   from the debugger.
3. Open the Simplicity Studio's Energy Profiler.  Select Start Energy Capture
   from the Profiler menu.
4. Zoom in on the Y-axis (current) and observe the change in current draw
   each time the PB0 push button is pressed to toggle between VS2 and VS1.

================================================================================

Peripherals Used:
CMU    - FSRCO @ 20 MHz, HFRCO @ 38MHz

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22C224F512
PB00  - push button PB0

Board: Silicon Labs EFR32FG23 868-915 MHz 14 dBm Board (BRD4263B)
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG23A010F512
PB01  - push button PB0

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24A010F1536GM48
PB01  - push button PB0