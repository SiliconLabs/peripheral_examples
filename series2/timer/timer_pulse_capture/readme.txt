timer_pulse_capture

This project demonstrates single pulse capture using the TIMER module. The HFXO
is configured to run the HFPER clock. TIMER is then initialized for input
capture on Compare/Capture channel 0 for interrupts on every edge. The GPIO Pin
specified below is to be connected to a periodic signal or pulse generator. The
two edges captured (one falling and one rising) are read from the CCV register.

Note: The range of frequencies this program can measure accurately is limited
due to dropout at higher frequencies, input setup time, and the HPERCLK frequency
selected for the timer peripheral source. The minimum measurable pulse width is 
~10X HPERCLK period.

================================================================================

Peripherals Used:
TIMER - CC0
HFXO - 38.4 MHz for radio boards xG21
================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Connect a periodic signal to GPIO pin specified below
3. Go into debug mode and click run
4. View the firstEdge and secondEdge global variables in the watch window

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA6 - TIM0_CC0 (Expansion Header Pin 14)