frequency_generation

This readme applies for Giant Gecko, Wonder Gecko
Leopard Gecko, Zero Gecko, and Happy Gecko

This project demonstrates frequency generation using the TIMER module.
TIMER1 is initialized for output compare on Compare/Capture channel 0 which
is routed to GPIO Pin PD6 (Expansion Header Pin 16). The Top value is set
such that on each overflow, the output toggles at the desired frequency.

NOTE: The range of frequencies the program can generate is limited. The maximal
frequency is 1/2 the clock frequency (Setting the top value to 0 causes an 
overflow on each clock cycle). The minimum frequency is
(clock frequency) / (2 * 65535 * prescale). By default this is 
(14 * 10^6) / (2 *65535 * 1) = 107Hz.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure waveform on PD6 (Expansion Header Pin 16)

Peripherals Used:
HFRCO  - 14 MHz
TIMER1 - CC0

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4
