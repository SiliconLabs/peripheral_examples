pulse_width_modulation_DMA

This readme applies for main_S0

This project demonstrates DMA driven pulse width modulation using the TIMER
module. TIMER0 is initialized for PWM on Compare/Capture channel 0 which is
routed to GPIO Pin PC10 (Expansion Header Pin 16). In PWM mode, overflow events
set the output pin, while compare events clear the pin. Thus the overflow value
is set to output the desired signal frequency, while the CCV is set to control 
the duty cycle. The uDMA is set to loop through a buffer of duty cycles
incrementing from 0 to 100%, and writes to the CCVB on each compare event.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PC10 (Expansion Header Pin 16)
3. Observe 1 KHz signal continuously varying duty cycle

Peripherals Used:
HFRCO  - 14 MHz
TIMER0 - CC0
uDMA   - Ping Pong transfer

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD6 (Expansion Header Pin 6)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD6 (Expansion Header Pin 6)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4
