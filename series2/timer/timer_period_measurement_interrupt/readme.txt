timer_period_measurement_interrupt

This project demonstrates period measurement using the TIMER module. TIMER is
initialized for input capture on Compare/Capture channel 0 for falling edge
capture and interrupts. Upon each interrupt, the captured edge is read from CC0
and compared with the previously captured edge to calculate the period. The
measured period is stored in the measuredPeriod global variable in units of 
microseconds.

Example: for a 1 kHz input signal, the measuredPeriod variable will equal 1000.

Note: The range of frequencies this program can measure accurately is limited.
The minimum measurable period is around 3 microseconds, or 333 kHz. At higher
frequencies accuracy is diminished due to dropout, and above 333 kHz the 
interrupt execution time is longer than the signal period. For methods to 
measure shorter periods, see the timer_pulse_capture example.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Connect a periodic signal to the GPIO pin specified below
3. Go into debug mode and click run
4. View the measuredPeriod (time period in us) global variable in the debugger

================================================================================

Peripherals Used:
CMU    - HFRCO @ 19 MHz
TIMER0 - HFPERCLK (19 MHz for series 2 boards)

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA6 - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PA6 - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA6 - TIM0_CC0 (Expansion Header Pin 11)
