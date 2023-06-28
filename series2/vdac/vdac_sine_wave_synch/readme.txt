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

Note: The BRD4270B's VDAC CH0 Main output is connected to an external pullup
resistor, capacitor, and pushbutton on the WSTK. To avoid interference from
these components, the EFR32xG25 example uses the VDAC CH0 Auxiliary output to 
output to a pin on the ABUS.

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

Board     | avg current EM1 | avg current EM0 (Enter EM1 code commented out)
================================================================================
BRD4204D  |          900 uA |          1100 uA
BRD4186C  |          680 uA |           840 uA
BRD4270B  |          740 uA |           940 uA
BRD4400C  |          866 uA |          1050 uA

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Use an oscilloscope to measure the output pin.

================================================================================

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz via EM01GRPCCLK
EMU
VDAC   - internal 1.25V reference, continuous mode

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32ZG23 Starter Kit (BRD4204D)
Device: EFR32ZG23B010F512IM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)

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