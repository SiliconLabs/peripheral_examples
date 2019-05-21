
This project demonstrates polled period measurement using the TIMER module. 
TIMER is initialized for input capture on Compare/Capture channel 0 for falling 
edge capture. The program waits for a falling edge and then the captured edge is 
read from CC0 and compared with the previously captured edge to calculate the 
period. The measured period is stored in the measuredPeriod global variable in 
units of microseconds.

Example: for a 1 kHz input signal, the measuredPeriod variable will equal 1000.

Note: The range of frequencies this program can measure accurately is limited.
The minimum measurable period is around 3 microseconds, or 333 kHz. At higher
frequencies accuracy is diminished due to dropout, and above 333 kHz the 
interrupt execution time is longer than the signal period. For methods to 
measure shorter periods, see the timer_pulse_capture example.

================================================================================

Peripherals Used:
TIMER0 - HFPERCLK (19 MHz for series 1 boards)

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Connect a periodic signal to the GPIO pin specified below
3. Go into debug mode and click run
4. View the measuredPeriod global variable in the debugger

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)