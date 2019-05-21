This project demonstrates pulse width modulation using the TIMER module. TIMER 
is initialized for PWM on Compare/Capture channel 0 which is routed to the GPIO 
Pin specified below. In PWM mode, overflow events set the output pin, while 
compare events clear the pin. Thus the overflow value is set to output the 
desired signal frequency, while the CCV is set to control the duty cycle. The 
duty cycle is altered by writing to the CCVB register, which safely updates the 
compare value on the next overflow event.

================================================================================

Peripherals Used:
TIMER0/1 - HFPERCLK (19 MHz for series 2 boards)

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to view the 1 KHz signal with 30% duty cycle on the GPIO 
   pin specified below

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)