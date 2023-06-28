vdac_sample_off

This project uses the VDAC in sample/off mode with single-ended output to output
0.5V to a pin in EM3. The VDAC operates independently from the core. Since the
VDAC is operating in sample/off mode, the output voltage is only driven by the
VDAC during the output hold time. Utilizing the VDAC peripheral in sample/off 
mode may require additional external components (bulk capacitor or low-pass RC 
filter, e.g.) depending on the intended application. Suitable filter design and 
VDAC refresh rate and hold times can be adjusted based on the amount of output
ripple tolerated by the application.

Note: The BRD4270B's VDAC CH0 Main output is connected to an external pullup
resistor, capacitor, and pushbutton on the WSTK. To avoid interference from
these components, the EFR32xG25 example uses the VDAC CH0 Auxiliary output to 
output to a pin on the ABUS.

Approximate current consumption measurements are provided below using Simplicity
Studio's built-in energy profiler. Projects were built with the default import
configuration - Debug build configuration (-g3) and no optimization (gcc -O0). 
Refer to device specific datasheets for more details about low energy mode 
currents. This example has a refresh rate of every 32 clock cycles. Here are 
some energy measurements for several different refresh rates. Although using a
slower refresh rate uses less energy, this example uses a fixed output hold time
and thus the voltage can dissipate between refresh cycles depending on external
circuit (filter) and load.

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

          | avg current consumption in EM3; refresh rates in VDAC_REFRESH_CLKs
Board     | 32 clks | 64 clks | 128 clks | 256 clks
================================================================================
BRD4186C  |   22 uA |   17 uA |    14 uA |    13 uA
BRD4270B  |   17 uA |   11 uA |     8 uA |     7 uA 
BRD4400C  |   16 uA |   10 uA |     8 uA |     6 uA

RMS output|  380 mV |  265 mV |   185 mV |   130 mV

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Use an oscilloscope to measure the VDAC output pin with respect to ground.

================================================================================

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz, LFRCO @ 32.768 kHz via EM23GRPACLK
EMU
USART  - used only to power down onboard SPI flash
VDAC   - internal 1.25V reference, sample/off mode

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PA06 -  VDAC0 CH0 Auxiliary Output (Pin 8 of breakout pads)

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB00 -  VDAC0 CH0 Main Output (WSTK Breakout Pad J101 P15)