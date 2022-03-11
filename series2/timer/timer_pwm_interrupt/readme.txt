timer_pwm_interrupt

This project demonstrates pulse width modulation using the TIMER module. TIMER 
is initialized for PWM on Compare/Capture channel 0 which is routed to the GPIO 
Pin specified below. In PWM mode, overflow events set the output pin, while 
compare events clear the pin. Thus the overflow value is set to output the 
desired signal frequency, while the CCV is set to control the duty cycle. The 
duty cycle is altered by writing to the CCVB register, which safely updates the 
compare value on the next overflow event.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to view the 1 KHz signal with 30% duty cycle on the GPIO 
   pin specified below

================================================================================

Peripherals Used:
CMU    - HFRCO @ 19 MHz
TIMER0 - HFPERCLK (19 MHz for series 2 boards)

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA6 - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PA6 - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA6 - TIM0_CC0 (Expansion Header Pin 11)
