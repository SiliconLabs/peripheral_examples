
This project demonstrates the generation of a single pulse signal using output
compare. TIMER is initialized for output compare on Compare/Capture channel 0,
which is routed to the GPIO Pin specified below. The buffered compare values are
set such that a single pulse is generated.

================================================================================

Peripherals Used:
TIMER0/1 - HFPERCLK (19 MHz for series 2 boards)

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to measure the GPIO pin specified below
3. A 1 s pulse should be generated after 3.5 seconds

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)