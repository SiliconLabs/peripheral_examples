single_pulse_generation_interrupt

This project demonstrates the generation of a single pulse using output
compare. WTIMER0 is initialized for output compare on Compare/Capture channel 0
which is routed to GPIO Pin PC10 (Expansion Header Pin 16). The wtimer is 
configured in one-shot mode and an interrupt triggers after the first compare
to load the second, falling edge to the CCV. The values loaded into the CCV are
such that a 1ms pulse is generated after a 3 second delay.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PC10 (Expansion Header Pin 16)
3. a 1 ms pulse will be generated after 3 seconds

Peripherals Used:
HFRCO  - 19 MHz
WTIMER0 - CC0

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30
