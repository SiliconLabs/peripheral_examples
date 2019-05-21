This project demonstrates single output compare using the TIMER module.
TIMER1 is initialized for output compare on Compare/Capture channel 0 which
is routed to the GPIO pin specified below. The Compare value is set such that
after 3 seconds, the GPIO pin will be set high.

================================================================================

Peripherals Used:
TIMER0 - HFPERCLK (19 MHz for series 2 boards)

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to measure the GPIO pin specified below
3. The pin should go high after 3 seconds

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)