single_edge_output_compare

This readme applies for Giant Gecko, Leopard Gecko and Happy Gecko

This project demonstrates single output compare using the TIMER module.
TIMER1 is initialized for output compare on Compare/Capture channel 0 which
is routed to GPIO Pin PD6 (Expansion Header Pin 16). The Compare value is
set such that after 3 seconds, PD6 will be set high.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PD6 (Expansion Header Pin 16)
3. Pin will go high after 3 seconds


Peripherals Used:
HFRCO  - 14 MHz
TIMER1 - CC0

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD6 (Expansion Header Pin 16)  - Output TIM1_CC0 #4

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD6 (Expansion Header Pin 16)  - Output TIM1_CC0 #4

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 (Expansion Header Pin 16)  - Output TIM1_CC0 #4

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD6 (Expansion Header Pin 16)  - Output TIM1_CC0 #4

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 (Expansion Header Pin 16)  - Output TIM1_CC0 #4
