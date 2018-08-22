single_pulse_generation_DMA

This readme applies for Blue Gecko, Flex Gecko, 
Mighty Gecko, and Pearl Gecko

This project demonstrates the generation of a single pulse using output
compare and the LDMA. TIMER0 is initialized for output compare on Compare/Capture 
channel 0 which is routed to GPIO Pin PC10 (Expansion Header Pin 16). The timer
is configured in one-shot mode and an interrupt triggers after the first 
compare to load the second, falling edge to the CCV. The LDMA is configured to
transfer a single word to the CCV register once the first compare event occurs.
The values loaded into the CCV are such that a 1ms pulse is generated after a 3
second delay.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PC10 (Expansion Header Pin 16)
3. a 1 ms pulse will be generated after 3 seconds

Peripherals Used:
HFRCO  - 19 MHz
TIMER0 - CC0
LDMA   - SINGLE_M2P_BYTE

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 (Expansion Header Pin 16) - Output TIM0_CC0 #15
