vdac_sine_wave_synch

This project uses the VDAC and the internal sine wave generator to produce a
16-point sine wave at a frequency of f_sinewave Hz centered at the reference
voltage divided by 2. This project operates in EM1. By default, this program
outputs a sinewave at 31.25 kHz.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

Note: The sine wave will always be output on Channel 0 but Channel 1 can still 
be used independently as a single-ended DAC output. Channel 1 settings are 
ignored if the differential mode is enabled.

Note: If the differential mode is enabled, the sine wave will be output on both 
channels, but inverted. 

Note that the DAC_CLK can only be programmed to a maximum of 1 MHz. In this 
example, f_IN_CLK is the EM01GRPACLK. The default value of the EM01GRPACLK is 
19 MHz.
f_DAC_CLK = f_IN_CLK / (PRESCALE + 1)

The equation below shows how the calculation for the sinewave frequency.
f_sinewave = f_EM01GRPACLK / (32 * (PRESCALE + 1))
The prescaler bitfield is 7 bits so the actual prescaler can be any number 
between 1 and 128. The prescaler must be chosen, as mentioned above, to ensure 
that the DAC_CLK is less than or equal to 1 MHz.

Although the VDAC can operate in EM3, synchronous reflexes are clocked on the 
EM01GRPACLK so they do not work in EM2/3 (high frequency clocks get turned off 
in EM2/3).

This example used approximately 900 uA. After commenting out the line of code 
that puts the board in EM1, this example used approximately 1.1 mA.
Note: this energy measurement was done using Simplicity Studio's
built-in energy profiler for BRD4263B with a Debug build configuration and
optimization gcc -g3.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Use an oscilloscope to measure the output pin.

================================================================================

Peripherals Used:
HFRCODPLL - 19 MHz
VDAC  - internal 1.25V reference, continuous mode

================================================================================

Listed below are the devices that do not have a VDAC module
 - EFR32xG21
 - EFR32xG22
 
================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32FG23 Starter Kit (BRD4263B)
Device: EFR32FG23A010F512GM48
PB0 - VDAC0 CH0 Main Output (Pin 15 of breakout pads)
