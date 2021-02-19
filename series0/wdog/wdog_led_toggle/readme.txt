wdog_led_toggle

This project demonstrates the functionality of the watchdog timer. It uses 
an LED and a Push-button to indicate the state of the system. If the LED is
flashing, the system has not undergone a reset. If the Push Button 0 is 
pressed and held for some time, it will trigger a WDOG reset. The code looks
for a reset source in the beginning of the execution and if WDOG is found to 
be the reset source, the LED is turned on and the code does not execute 
any further. 

How To Test:
1. Build the project and download to the Starter Kit
2. LED 0 should be blinking
3. Press and hold Push Button 0 for a period of 3 seconds
4. The LED is now ON and not blinking anymore, which is indicative of
   a WDOG reset

Peripherals Used:
WDOG - 2 seconds period


Board: Silicon Labs EFM32G Starter Kit (STKXXX)
Device: EFM32G890F128
PC0 - LED0
PB9 - Push Button PB0

Board: Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD7 - LED0
PD8 - Push Button PB0

Board: Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE2 - LED0
PB9 - Push Button PB0

Board: Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PC8 - Push Button PB0

Board: Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE2 - LED0
PB9 - Push Button PB0

Board: Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PB9 - Push Button PB0

Board: Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222f32
PC10 - LED0
PC8 - Push Button PB0

