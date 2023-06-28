vdac_timer_dma_waveform

This project uses the DAC/VDAC and TIMER0 to output a 32 point sine wave at a
particular frequency (10 kHz by default). The project uses the LDMA to write to 
the CH0F buffer. This project operates in EM1 because the timer can't operate in 
EM2/EM3.

Note: The BRD4270B's VDAC CH0 Main output is connected to an external pullup
resistor, capacitor, and pushbutton on the WSTK. To avoid interference from
these components, the EFR32xG25 example uses the VDAC CH0 Auxiliary output to 
output to a pin on the ABUS.

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
BRD4204D  |          950 uA |          1200 uA
BRD4186C  |          760 uA |           920 uA
BRD4270B  |          860 uA |          1065 uA
BRD4400C  |          925 uA |          1140 uA

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Measure the VDAC output pin with respect to ground

================================================================================

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz via EM01GRPCCLK
EMU
LDMA   - memory to peripheral data transfer
TIMER  - TIMER0 @ 320 kHz (WAVEFORM_FREQ * SINE_TABLE_SIZE)
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