
This project demonstrates the generation of a single pulse using output
compare and the LDMA. TIMER0 is initialized for output compare on Compare/Capture 
channel 0 which is routed to GPIO Pin specified below. The timer is configured 
in one-shot mode and an interrupt triggers after the first compare to load the 
second, falling edge to the OC. The LDMA is configured to transfer a single 
word to the OC register once the first compare event occurs. The values loaded 
into the OC are such that a 1ms pulse is generated after a 3 second delay.

================================================================================

Peripherals Used:
HFRCO  - 19 MHz
TIMER0 - CC0
LDMA   - SINGLE_M2P_BYTE

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