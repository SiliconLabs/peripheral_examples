pulse_capture

This readme applies for Blue Gecko, Flex Gecko, 
Mighty Gecko, and Pearl Gecko

This project demonstrates single pulse capture using the TIMER module.
The HFXO, a 40 MHz oscillator, is configured to run the HFPER clock. TIMER0 is 
then initialized for input capture on Compare/Capture channel 0 for interrupt
on every second falling edge.  GPIO Pin PC10 (Expansion Header Pin 16) is to be
connected to a periodic signal or pulse generator, and two falling edges are 
captured and read from the CC0 register.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PC10
3. View the first_edge and second_edge global variables in the debugger

NOTE: The minimum duration pulse the Compare/Capture channel can accurately
measure is limited to around 700ns or 1.43 MHz. 

Peripherals Used:
HFXO  - 40 MHz
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