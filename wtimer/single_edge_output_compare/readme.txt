single_edge_output_compare

This project demonstrates single output compare using the WTIMER module.
WTIMER0 is initialized for output compare on Compare/Capture channel 0 which
is routed to GPIO Pin PC10 (Expansion Header Pin 16). The Compare value is
set such that after 3 seconds, PC10 will be set high.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PC10 (Expansion Header Pin 16)
3. Pin will go high after 3 seconds

Peripherals Used:
HFRCO  - 19 MHz
WTIMER0 - CC0

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PC10 (Expansion Header Pin 16) - Output WTIM0_CC0 #30