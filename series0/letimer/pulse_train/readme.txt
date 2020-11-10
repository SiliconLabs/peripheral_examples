pulse_train

This project demonstrates pulse train generation using the LETIMER.
The project initializes the timer in free mode, and creates a one 
LFCLK length pulse on each underflow event. The underflow occurs 
at 1 kHz frequency.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure waveform on the appropriate pin listed below

Peripherals Used:
LETIMER0 - free mode


Board:  Silicon Labs EFM32G Starter Kit (G8xxx-STK)
Device: EFM32G890F128
PD6 (Expansion Header Pin 16) - Input LET0_O0 #0

Board:  Silicon Labs EFM32GG Starter Kit (STK3700A)
Device: EFM32GG990F1024
PD6 (Expansion Header Pin 16) - Input LET0_O0 #0

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 (Expansion Header Pin 16) - Input LET0_O0 #0

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD6 (Expansion Header Pin 16) - Input LET0_O0 #0

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 (Expansion Header Pin 16) - Input LET0_O0 #0
