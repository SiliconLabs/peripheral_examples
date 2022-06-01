vdac_sample_off

This project uses the VDAC in sample/off mode with single-ended output to output
0.5V to a pin in EM3. The VDAC operates independently from the core. Since the
VDAC is operating in sample/off mode, the output voltage is only driven by the
VDAC during the output hold time. Utilizing the VDAC peripheral in sample/off 
mode may require additional external components (bulk capacitor or low-pass RC 
filter, e.g.) depending on the intended application. Suitable filter design and 
VDAC refresh rate and hold times can be adjusted based on the amount of output
ripple tolerated by the application.

Approximate current consumption measurements are provided below using Simplicity
Studio's built-in energy profiler. Projects were built with the default import
configuration - Debug build configuration (-g3) and no optimization (gcc -O0). 
Refer to device specific datasheets for more details about low energy mode 
currents. This example has a refresh rate of every 32 clock cycles. Here are 
some energy measurements for several different refresh rates. Although using a
slower refresh rate uses less energy, this example uses a fixed output hold time
and thus the voltage can dissipate between refresh cycles depending on external
circuit (filter) and load.

          | avg current consumption in EM3; refresh rates in VDAC_REFRESH_CLKs
Board     | 32 clks | 64 clks | 128 clks | 256 clks
================================================================================
BRD4186C  |   22 uA |   17 uA |    14 uA |    13 uA 

RMS output|  380 mV |  265 mV |   185 mV |   130 mV

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

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

Listed below are the devices that do not have a VDAC module
 - EFR32xG21
 - EFR32xG22
 
================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)