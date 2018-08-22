frequency_generation

This readme applies for the Gecko device.

This project demonstrates frequency generation using the TIMER module.
TIMER0 is initialized for output compare on Compare/Capture channel 0 which
is routed to GPIO Pin PD1 (Expansion Header Pin 6). The Top value is set
such that on each overflow, the output toggles at the desired frequency.

NOTE: The range of frequencies the program can generate is limited. The maximal
frequency is 1/2 the clock frequency (Setting the top value to 0 causes an 
overflow on each clock cycle). The minimum frequency is
(clock frequency) / (2 * 65535 * prescale). By default this is 
(14 * 10^6) / (2 *65535 * 1) = 107Hz.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure waveform on PC10 (Expansion Header Pin 16)

Peripherals Used:
HFRCO  - 14 MHz
TIMER0 - CC0

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 (Expansion Header Pin 6) - Input TIM0_CC0 #3