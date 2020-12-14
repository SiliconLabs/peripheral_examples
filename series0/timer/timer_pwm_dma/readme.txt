timer_pwm_dma

This project demonstrates DMA driven pulse width modulation using the TIMER
module. TIMER0 is initialized for PWM on Compare/Capture channel 0 which is
routed to the GPIO Pin specified below. In PWM mode, overflow events
set the output pin, while compare events clear the pin. Thus the overflow value
is set to output the desired signal frequency, while the CCV is set to control 
the duty cycle. The DMA is set to loop through a buffer of duty cycles
incrementing from 0 to 100%, and writes to the CCVB on each overflow event.

================================================================================

Peripherals Used:
TIMER0/1 - HFPERCLK (14 MHz)
DMA

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to view a 1 kHz signal with continuosly varying duty
   cycle on the GPIO pin specified below

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
