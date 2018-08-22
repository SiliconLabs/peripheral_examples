rtc_ulfrco

This project demonstrates the use of the RTC in EM3.
The RTC is set up and then the device goes into EM3.
After 20 seconds the RTC interrupts waking the device
and causing LED's 0 and 1 to toggle.

How To Test:
1. Build the project and download to the Starter Kit
2. Observe the LED's

Peripherals Used:
ULFRCO  - 1 kHz
RTC COMP0

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
RTC - COMP0
PC11 - LED1

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
RTC - COMP0
PF5 - LED1

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
RTC - COMP0
PE3 - LED1

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
RTC - COMP0
PE3 - LED1

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
RTC - COMP0
PE3 - LED1
