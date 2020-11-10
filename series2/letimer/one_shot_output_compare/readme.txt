letimer_one_shot_output_compare

This project demonstrates output compare in EM2 using the LETIMER.
The project initializes the letimer in one-shot mode, and expansion 
header pin toggles REPEAT_COUNT times while staying in a low energy mode.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure output on the appropriate pin listed below

Peripherals Used:
LETIMER0 - one-shot mode


Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - LET0_O0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA6 - LET0_O0 (Expansion Header Pin 14)