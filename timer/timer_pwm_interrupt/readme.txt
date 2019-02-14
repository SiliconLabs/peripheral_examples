
This project demonstrates pulse width modulation using the TIMER module. TIMER 
is initialized for PWM on Compare/Capture channel 0 which is routed to the GPIO 
Pin specified below. In PWM mode, overflow events set the output pin, while 
compare events clear the pin. Thus the overflow value is set to output the 
desired signal frequency, while the CCV is set to control the duty cycle. The 
duty cycle is altered by writing to the CCVB register, which safely updates the 
compare value on the next overflow event.

================================================================================

Peripherals Used:
TIMER0/1 - HFPERCLK (14 MHz for series 0 boards) (19 MHz for series 1 boards)

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to view the 1 KHz signal with 30% duty cycle on the GPIO 
   pin specified below

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 - TIM0_CC0 #3 (Expansion Header Pin 6)

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD6 - TIM1_CC0 #4 (Expansion Header Pin 6)

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 - TIM0_CC0 #15 (Expansion Header Pin 16)

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC13 - TIM1_CC0 #0 (Breakout Pads Pin 23)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC13 - TIM1_CC0 #0 (Expansion Header Pin 9)

