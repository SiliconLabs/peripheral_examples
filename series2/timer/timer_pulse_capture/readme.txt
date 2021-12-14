timer_pulse_capture

This project demonstrates single pulse capture using the TIMER module. The HFXO
is configured to run the HFPER clock. TIMER is then initialized for input
capture on Compare/Capture channel 0 for interrupts on every edge. The GPIO Pin
specified below is to be connected to a periodic signal or pulse generator. The
two edges captured (one falling and one rising) are read from the CCV register.

Note: The range of frequencies this program can measure accurately is limited.
The minimum measurable period is around 700 ns, or 1.43 MHz.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Connect a periodic signal to GPIO pin specified below
3. Go into debug mode and click run
4. View the firstEdge and secondEdge global variables in the watch window

================================================================================

Peripherals Used:
TIMER - CC0

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
CMU - HFXO @ 38.4 MHz
PA6 - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
CMU - HFXO @ 38.4 MHz
PA6 - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
CMU - HFXO @ 39 MHz
PA6 - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24A010F1536GM48
CMU - HFXO @ 39 MHz
PA6 - TIM0_CC0 (Expansion Header Pin 11)
