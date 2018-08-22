single_edge_output_compare

This readme applies for Gecko

This project demonstrates single output compare using the TIMER module.
TIMER0 is initialized for output compare on Compare/Capture channel 0 which
is routed to GPIO Pin PD1 (Expansion Header Pin 6). The Compare value is
set such that after 3 seconds, PD1 will be set high.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PD1 (Expansion Header Pin 6)
3. Pin will go high after 3 seconds

Peripherals Used:
HFRCO  - 14 MHz
TIMER0 - CC0

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 (Expansion Header Pin 6) - Output TIM0_CC0 #3
