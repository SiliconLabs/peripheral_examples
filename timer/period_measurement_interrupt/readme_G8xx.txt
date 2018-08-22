period_measurement_interrupt

This readme applies for the Gecko device.

This project demonstrates period measurement using the TIMER module. TIMER0 is
initialized for input capture on Compare/Capture channel 0 for falling edge 
capture and interrupts. GPIO Pin PD1 (Expansion Header Pin 6) is to be
connected to a periodic signal, and edges captured from PD1 are stored in CC0.
Upon each interrupt, the captured edge is read from CC0 and compared with the
previously captured edge to calculate the period. The measured period is stored
in a global variable.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PD1 (Expansion Header Pin 6)
3. View the measured_period global variable in the debugger

NOTE: The range of frequencies this program can measure accurately is limited.
The minimum measurable period is around 3 microseconds, or 333 kHz. At higher
frequencies accuracy is diminished due to dropout, and above 333 kHz the 
interrupt execution time is longer than the signal period. For methods to 
measure shorter periods, see the pulse_capture example.

Peripherals Used:
HFRCO  - 14 MHz
TIMER0 - CC0

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 (Expansion Header Pin 6) - Input TIM0_CC0 #3