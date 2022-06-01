vdac_timer_dma_waveform

This project uses the DAC/VDAC and TIMER0 to output a 32 point sine wave at a
particular frequency (10 kHz by default). The project uses the LDMA to write to 
the CH0F buffer. This project operates in EM1 because the timer can't operate in 
EM2/EM3.

Approximate current consumption measurements are provided below using Simplicity
Studio's built-in energy profiler. Projects were built with the default import
configuration - Debug build configuration (-g3) and no optimization (gcc -O0). 
Refer to device specific datasheets for more details about low energy mode 
currents.

Board     | avg current EM1 | avg current EM0 (Enter EM1 code commented out)
================================================================================
BRD4263B  |          950 uA |          1200 uA
BRD4186C  |          760 uA |           920 uA

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

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

Listed below are the devices that do not have a VDAC module
 - EFR32xG21
 - EFR32xG22

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32FG23 Starter Kit (BRD4263B)
Device: EFR32FG23A010F512GM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB00 -  VDAC0 CH0 Main Output (Pin 15 of breakout pads)