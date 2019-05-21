
This project demonstrates the generation of a single pulse using output compare.
TIMER is initialized for output compare on Compare/Capture channel 0 which is
routed to the GPIO Pin specified below. The timer is configured in one-shot mode
and toggles the output at every compare event. The first compare event is
configured to trigger after NUM_SEC_DELAY seconds. This compare event toggles
the output high and triggers an interrupt that loads the CCV compare register
with another compare value. The second compare event is set to occur PULSE_WIDTH
s after the first compare event and toggles the output low.

================================================================================

Peripherals Used:
TIMER0/1 - HFPERCLK (19 MHz for series 2 boards)

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to measure the GPIO pin specified below
3. A 1 s pulse should be generated after 1 second

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)