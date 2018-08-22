single_pulse_generation_polled

This readme applies for Blue Gecko, Flex Gecko, 
Mighty Gecko, and Pearl Gecko

This project demonstrates the generation of a single pulse signal using output
compare. TIMER0 is initialized for output compare on Compare/Capture channel 0
which is routed to GPIO Pin PC10 (Expansion Header Pin 16). The buffered 
compare values are set such that a single 1 pulse is generated.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PC10 (Expansion Header Pin 16)
3. a 1 ms pulse will be generated after 3.5 milliseconds

Peripherals Used:
HFRCO  - 19 MHz
TIMER0 - CC0

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 (Expansion Header Pin 16) - Output TIM0_CC0 #15
