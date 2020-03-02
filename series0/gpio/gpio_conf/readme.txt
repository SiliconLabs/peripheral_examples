gpio_conf

This project demonstrates how GPIO pin configuration, reading, and writing are
performed. By default, LED0 is configured to be driven with the default drive
strength. When the PB0 button is pressed, the EFM32 will change the drive
strength of LED0 to its lowest value.  Default and alternate drive strength are
derivative-specific; check the datasheet for the device in question for drive
strength options.

How To Test:
1. Build the project and download to the Starter Kit
2. Press down on PB0 and see the LED0 dim
3. Release PB0 and see the LED get brighter

Peripherals Used:
HFRCO - 14 MHz

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - LED0
PB9 - Push Button PB0

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE2 - LED0
PB9 - Push Button PB0

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PC9 - Push Button PB0

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE2 - LED0
PB9 - Push Button PB0

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD7 - LED0
PD8 - Push Button PB0

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PB9 - Push Button PB0

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC10 - LED0
PC8 - Push Button PB0
