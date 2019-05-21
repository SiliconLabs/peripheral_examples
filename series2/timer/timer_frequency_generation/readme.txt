This project demonstrates frequency generation using the TIMER module.
TIMER0 is initialized for output compare on Compare/Capture channel 0 which
is routed to GPIO Pin PC10 (Expansion Header Pin 16). The Top value is set
such that on each overflow, the output toggles at the desired frequency.

NOTE: The range of frequencies the program can generate is limited. The maximal
frequency is 1/2 the clock frequency (Setting the top value to 0 causes an 
overflow on each clock cycle). The minimum frequency is
(clock frequency) / (2 * 65535 * prescale). By default this is 
(19 * 10^6) / (2 *65535 * 1) = 145Hz.

================================================================================

Peripherals Used:
TIMER0 - HFPERCLK (19 MHz for series 2 boards)

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Measure waveform on PA6 (Expansion Header Pin 14)

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)