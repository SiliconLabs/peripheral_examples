pulse_width_modulation_DMA

This readme applies for main_S1

This project demonstrates DMA driven pulse width modulation using the TIMER
module. TIMER0 is initialized for PWM on Compare/Capture channel 0 which is
routed to GPIO Pin PC10 (Exp. Header pin below). In PWM mode, overflow events
set the output pin, while compare events clear the pin. Thus the overflow value
is set to output the desired signal frequency, while the CCV is set to control 
the duty cycle. The LDMA is set to loop through a buffer of duty cycles
incrementing from 0 to 100%, and writes to the CCVB on each compare event.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PC10 (Exp. Header pin below)
3. Observe 1 KHz signal continuously varying duty cycle

Peripherals Used:
HFRCO  - 19 MHz
TIMER0 - CC0
LDMA   - LINKREL_M2P_BYTE

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
