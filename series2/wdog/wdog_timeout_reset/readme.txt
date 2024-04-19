wdog_timeout_reset

This project demonstrates the functionality of the watchdog timer. It uses
LED0 and a Push-button to indicate the state of the system. LED0 will flash as
long as Push Button 0 is not pressed and the system has not undergone a 
watchdog reset. While Push Button 0 is pressed, LED0 will stop flashing. If
Push Button 0 is pressed and held through the WDOG timeout period, the watchdog
will trigger a reset. The code looks for a reset source in the beginning of the
execution and if WDOG is found to be the reset source, LED0 is turned on and the
code does not execute any further.

Note: For the EFR32xG27 Radio Board, this example uses LED1 instead of LED0.
Also, the radio board connects Push Button 0 to LED0 via hardware, so LED0 will
be on while Push Button 0 is pressed and off while Push Button 0 is not
pressed.

How To Test:
1. Build the project and download to the Starter Kit
2. LED should be blinking
3. Press and hold Push Button 0 for 3 seconds
4. LED is now on and not blinking anymore, which indicates a WDOG reset

Peripherals Used:
WDOG - 2 second period


Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PB00  - LED0
PD03  - Push Button PB1

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22A224F512IM40
PD02  - LED0
PB01  - Push Button PB1

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02  - LED0
PB03  - Push Button PB1

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02  - LED0
PB03  - Push Button PB1

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06  - LED0
PB01  - Push Button PB1

Board:  Silicon Labs EFR32xG27 Buck Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00  - LED0
PB01  - Push Button PB1

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02  - LED0
PB03  - Push Button PB1
