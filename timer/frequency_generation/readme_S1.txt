frequency_generation

This readme applies for Blue Gecko, Flex Gecko, 
Mighty Gecko, and Pearl Gecko

This project demonstrates frequency generation using the TIMER module.
TIMER0 is initialized for output compare on Compare/Capture channel 0 which
is routed to GPIO Pin PC10 (Expansion Header Pin 16). The Top value is set
such that on each overflow, the output toggles at the desired frequency.

NOTE: The range of frequencies the program can generate is limited. The maximal
frequency is 1/2 the clock frequency (Setting the top value to 0 causes an 
overflow on each clock cycle). The minimum frequency is
(clock frequency) / (2 * 65535 * prescale). By default this is 
(19 * 10^6) / (2 *65535 * 1) = 145Hz.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure waveform on PC10 (Expansion Header Pin 16)

Peripherals Used:
HFRCO  - 19 MHz
TIMER0 - CC0

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15