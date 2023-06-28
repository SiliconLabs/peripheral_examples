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

Note for Testing on xG28:
On BRD4400C, there is a diversity SPDT switch on the 2.4 GHz radio output that
will consume ~63 uA of additional current when supplied power via logic high on
GPIO PD02. The pin is configured in disabled mode (high-Z) by default, thus SPDT
switch is powered down by default. Be advised that when PD02 is driven to logic
high, observed current consumption will be higher than what is specified in the 
EFR32ZG28 device datasheet due to this additional integrated circuit.

Board     | avg current EM3 | avg current EM0 (Enter EM3 code commented out)
================================================================================
BRD4204D  |           35 uA |           950 uA
BRD4186C  |           38 uA |           725 uA
BRD4270B  |           36 uA |           885 uA
BRD4400C  |           33 uA |           916 uA

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

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32ZG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  VDAC0 CH0 Main Output (Pin 17 of breakout pads)

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB00 -  VDAC0 CH0 Main Output (WSTK Breakout Pad J101 P15)