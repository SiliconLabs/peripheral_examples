switch_led_interrupt

This project demonstrates how to use GPIO pins to trigger external
interrupts. If a push-button is pressed, and it is on an
odd pin, the odd GPIO interrupt is triggered and toggles
LED1. If the pin is on an even pin, the even GPIO interrupt
is triggered and toggles LED0.

How To Test:
1. Build the project and download to the Starter Kit
2. Press PB0 or PB1 to toggle the LEDs

Peripherals Used:
HFRCO  - 14 MHz

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - LED0
PC1 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PF5 - LED1
PC9 - Push Button PB0
PC10 - Push Button PB1

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD7 - LED0
PD8 - Push Button PB0
PB11 - Push Button PB1

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC10 - LED0
PC11 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1