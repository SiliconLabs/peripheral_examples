pulse_width_modulation_DMA

This readme applies for main_G8xx

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

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 (Expansion Header Pin 6) - Input TIM0_CC0 #3