timer_frequency_generation

This project demonstrates frequency generation using the TIMER module.
TIMER0 is initialized for output compare on Compare/Capture channel 0 which
is routed to GPIO Pin PA6 (see board specific pinout). The Top value is set
such that on each overflow, the output toggles at the desired frequency:
OUT_FREQ initially set to 1000 Hz.

NOTE: The range of frequencies the program can generate is limited. The maximal
frequency is 1/2 the clock frequency (Setting the top value to 0 causes an 
overflow on each clock cycle). The minimum frequency is
(clock frequency) / (2 * 65535 * prescale). By default this minimum frequency is 
(19 * 10^6) / (2 *65535 * 1) = 145Hz.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Measure waveform on PA6 (see board specific pinout below)

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
