pulse_width_modulation_interrupt

This readme applies for main_G8xx

This project demonstrates pulse width modulation using the TIMER module. TIMER0
is initialized for PWM on Compare/Capture channel 0 which is routed to GPIO Pin
PD1 (Exp. header pin below). In PWM mode, overflow events set the output 
pin, while compare events clear the pin. Thus the overflow value is set to
output the desired signal frequency, while the CCV is set to control the duty
cycle. The duty cycle is altered by writing to the CCVB register, which safely 
updates the compare value on the next overflow event.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure voltage on PC10 (Exp. header pin below)
3. Observe 1 KHz signal with 30% duty cycle

Peripherals Used:
HFRCO  - 14 MHz
TIMER0 - CC0

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 (Expansion Header Pin 6) - Input TIM0_CC0 #3
