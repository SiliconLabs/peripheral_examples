period_measurement_polled

This readme applies for Giant Gecko, Wonder Gecko
Leopard Gecko, Zero Gecko, and Happy Gecko

This project demonstrates polled period measurement using the TIMER module.
TIMER1 is initialized for input capture on Compare/Capture channel 0 for 
falling edge capture. GPIO Pin PD6 (Expansion Header Pin 16) is to be connected
to a periodic signal, and edges captured from PD6 are stored in CC0. The 
program waits until an edge is available. Then the captured edge is read from
CC0 and compared with the previously captured edge to calculate the period. 
The measured period is stored in a global variable.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PD6 (Expansion Header Pin 16)
3. View the measured_period global variable in the debugger

NOTE: The range of frequencies this program can measure accurately is limited.
The minimum measurable period is around 3 microseconds, or 333 kHz. At higher
frequencies accuracy is diminished due to dropout, and above 333 kHz the loop
execution time is longer than the signal period. For methods to measure shorter
periods, see the pulse_capture example.


Peripherals Used:
HFRCO  - 14 MHz
TIMER1 - CC0

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4
