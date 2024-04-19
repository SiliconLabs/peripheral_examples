wdog_warning_interrupt

This project demonstrates the warning interrupt functionality of the watchdog
timer. In this example, the watchdog triggers an interrupt when the counter
reaches 50% of the timer period. The timer period is set to 2048 ms, so the
watchdog must be cleared within 1024 ms. The main loop clears the watchdog
every 800 ms. When Push Button 1 is pressed, the watchdog clears after 1100 ms,
triggering the wdog warning interrupt. LED0 is turned on and the code does not
execute any further, causing the wdog timer to overflow and trigger a reset.
The code looks for a reset source in the beginning of the execution and if WDOG
is found to be the reset source, LED0 is turned on and the code does not
execute any further.

Note: For the EFR32xG27 Radio Board, Push Button 1 is connected to LED1 via
hardware, so LED1 will be on while Push Button 1 is pressed.

How To Test:
1. Build the project and download to the Starter Kit
2. LED should be blinking
3. Press Push Button 1
4. LED turns on, indicating a WDOG reset

Peripherals Used:
WDOG - 2 seconds period


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
