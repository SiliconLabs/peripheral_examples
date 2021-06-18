vdac_continuous

This project uses the VDAC in continuous mode with single-ended output to output
0.5V to a pin in EM3. The VDAC operates independently from the core. Since the
VDAC is operating in continuous mode, the output voltage will be stable even if
the output is loaded. The fact that the VDAC is continuously working will be
reflected by the power consumption.

This example uses approximately 35 uA on average. After commenting out the line
of code that puts the board in EM3, this example uses approximately 950 uA on
average. Note: this energy measurement was done using Simplicity Studio's
built-in energy profiler for BRD4263B with a Debug build configuration and
optimization gcc -g3. Refer to the datasheet for more details about low energy
mode currents.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Use an oscilloscope to measure the output pin.

================================================================================

Peripherals Used:
HFRCOEM23 - 19 MHz
VDAC      - internal 1.25V reference, continuous mode

================================================================================

Listed below are the devices that do not have a VDAC module
 - EFR32xG21
 - EFR32xG22
 
================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32FG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)
