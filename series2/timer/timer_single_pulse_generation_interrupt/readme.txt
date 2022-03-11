timer_single_pulse_generation_interrupt

This project demonstrates the generation of a single pulse using output compare.
TIMER is initialized for output compare on Compare/Capture channel 0 which is
routed to the GPIO Pin specified below. The timer is configured in one-shot mode
and toggles the output at every compare event. The first compare event is
configured to trigger after NUM_SEC_DELAY seconds. This compare event toggles
the output high and triggers an interrupt that loads the CCV compare register
with another compare value. The second compare event is set to occur PULSE_WIDTH
s after the first compare event and toggles the output low.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to measure the GPIO pin specified below
3. A 1 s pulse should be generated after 1 second

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
