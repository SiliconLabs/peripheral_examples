pulse_capture

This readme applies for Giant Gecko, Wonder Gecko
Leopard Gecko, Zero Gecko, and Happy Gecko

This project demonstrates single pulse capture using the TIMER module.
The HFXO, a 25 MHz oscillator, is configured to run the HFPER clock. TIMER1 is 
then initialized for input capture on Compare/Capture channel 0 for interrupt
on every second falling edge.  GPIO Pin PD6 (Expansion Header Pin 16) is to be
connected to a periodic signal or pulse generator, and two falling edges are 
captured and read from the CC0 register.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PD6
3. View the first_edge and second_edge global variables in the debugger

NOTE: The minimum duration pulse the Compare/Capture channel can accurately
measure is limited to around 700ns or 1.43 MHz. 

Peripherals Used:
HFXO  - 25 MHz
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
