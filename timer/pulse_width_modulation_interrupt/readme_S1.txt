pulse_width_modulation_interrupt

This readme applies for main_S1

This project demonstrates pulse width modulation using the TIMER module. TIMER0
is initialized for PWM on Compare/Capture channel 0 which is routed to GPIO Pin
PC10 (Exp. header pin outs below). In PWM mode, overflow events set the output 
pin, while compare events clear the pin. Thus the overflow value is set to
output the desired signal frequency, while the CCV is set to control the duty
cycle. The duty cycle is altered by writing to the CCVB register, which safely 
updates the compare value on the next overflow event.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PC10 (Exp. header pin outs below)
3. Observe 1 KHz signal with 30% duty cycle

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
