switch_led_polled

This project demonstrates a user how to set a simple digital input and digital
output. Button PB0 on the WSTK is an input. LED0 is the output.  When button 0
is pressed, LED0 will be on.

Note for EFR32xG21 devices, clock enabling is not required.

How To Test:
1. Build the project and download to the Starter Kit
2. Press PB0 to turn LED0 on
3. Release PB0 to turn LED0 off

Peripherals Used:
CMU    - FSRCO @ 20 MHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB0 - LED0
PD2 - Push Button PB0

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PD2 - LED0
PB0 - Push Button PB0

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB2 - LED0
PB1 - Push Button PB0

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24A010F1536GM48
PB2 - LED0
PB1 - Push Button PB0
