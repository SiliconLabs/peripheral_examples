switch_led_polled

This project demonstrates a user how to set a simple digital input and digital
output. Button PB0 on the WSTK is an input. LED0 is the output. When button 0
is pressed, LED0 will be on.

Note for the EFR32xG27 Radio Board, LED1 is the output. When button 0 is
pressed, LED1 will be on. The radio board connects button 0 to LED0 via
hardware, so LED0 will also be on while button 0 is pressed.

Note for EFR32xG21 devices, clock enabling is not required.

How To Test:
1. Build the project and download to the Starter Kit
2. Press PB0 to turn LED on
3. Release PB0 to turn LED off

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  LED0
PD02 -  Push Button PB0

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PD02 -  LED0
PB00 -  Push Button PB0

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 -  LED0
PB01 -  Push Button PB0

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02 -  LED0
PB01 -  Push Button PB0

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06 -  LED0
PB00 -  Push Button PB0

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB01 -  LED1
PB00 -  Push Button PB0

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 -  LED0
PB01 -  Push Button PB0