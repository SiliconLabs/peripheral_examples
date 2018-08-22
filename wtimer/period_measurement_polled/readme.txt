period_measurement_polled

This project demonstrates polled period measurement using the WTIMER module.
WTIMER0 is initialized for input capture on Compare/Capture channel 0 for 
falling edge capture. GPIO Pin PC10 (Expansion Header Pin 16) is to be 
connected to a periodic signal, and edges captured from PC10 are stored in CC0.
The program waits until an edge is available. Then the captured edge is read 
from CC0 and compared with the previously captured edge to calculate the 
period. The measured period is stored in a global variable.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PC10 (Expansion Header Pin 16)
3. View the measured_period global variable in the debugger

NOTE: The range of frequencies this program can measure accurately is limited.
The minimum measurable period is around 3 microseconds, or 333 kHz. At higher
frequencies accuracy is diminished due to dropout, and above 333 kHz the loop
execution time is longer than the signal period. For methods to measure shorter
periods, see the pulse_capture example.

Peripherals Used:
HFRCO  - 19 MHz
WTIMER0 - CC0

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30