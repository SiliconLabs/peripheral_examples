one_shot_output_compare

This project demonstrates output compare in EM2 using the LETIMER. The project
initializes the timer in one-shot mode, and after one second, while staying in 
a low energy mode, will set either Expansion Header Pin 15 or 16 depending on the board.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure output on the appropriate pin listed below

Peripherals Used:
LETIMER0 - one-shot mode

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 (Expansion Header Pin 16) - Input TIM0_CC0 #15

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 (Expansion Header Pin 16) - Input LET0_O0 #0

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 (Expansion Header Pin 16) - Input LET0_O0 #0

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A)
Device: EFM32MG1P232F256GM48
PC10 (Expansion Header Pin 15) - Input TIM0_CC0 #15

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PC10 (Expansion Header Pin 15) - Input TIM0_CC0 #15

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PC10 (Expansion Header Pin 15) - Input TIM0_CC0 #15

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A)
Device: EFM32FG1P133F256GM48
PC10 (Expansion Header Pin 15) - Input TIM0_CC0 #15

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PC10 (Expansion Header Pin 15) - Input TIM0_CC0 #15

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A)
Device: EFM32BG1P232F256GM48
PC10 (Expansion Header Pin 15) - Input TIM0_CC0 #15