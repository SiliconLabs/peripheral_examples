vdac_continuous

This project uses the VDAC in continuous mode with single-ended output to output
0.5V to a pin in EM3. The VDAC operates independently from the core. Since the
VDAC is operating in continuous mode, the output voltage will be stable even if
the output is loaded. The fact that the VDAC is continuously working will be
reflected by the power consumption.

Approximate current consumption measurements are provided below using Simplicity
Studio's built-in energy profiler. Projects were built with the default import
configuration - Debug build configuration (-g3) and no optimization (gcc -O0). 
Refer to device specific datasheets for more details about low energy mode 
currents.

Note: The energy profiler only measures the VMCU current. For BRD4270B, VMCU
only powers IOVDD and the serial flash. The 3.6V LDO powers the rest of the
radio board. To measure the 3.6V LDO current, replace R247 with ammeter
connections. The current provided below is the combination of the VMCU and 3.6V
current.

Board     | avg current EM3 | avg current EM0 (Enter EM3 code commented out)
================================================================================
BRD4263B  |           35 uA |           950 uA
BRD4186C  |           38 uA |           725 uA
BRD4270B  |           36 uA |           885 uA

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
CMU    - HFRCODPLL @ 19 MHz, HFRCOEM23 @ 19 MHz
EMU
USART  - used only to power down onboard SPI flash
VDAC   - internal 1.25V reference, continuous mode

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

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  VDAC0 CH0 Main Output (Pin 17 of breakout pads)