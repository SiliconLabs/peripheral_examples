wdog_prs_monitor

This project demonstrates the PRS monitoring function of the watchdog. The GPIO
is configured as an input and expects to receive a periodic signal with a
frequency > 1 Hz. The GPIO is connected to a PRS producer, and the WDOG is
connected to a PRS consumer of the same PRS channel. The main loop feeds the
watchdog every 1 second and toggles LED0. If the GPIO stops receiving the
periodic signal, a PRS event will not occur by the time the watchdog is fed, and
the watchdog will trigger a reset. Out of reset, the system checks the reset
cause. If the latest reset was caused by a watchdog timeout, LED0 turns on and
the code does not execute any further.

How To Test:
1. Connect the periodic external signal to the GPIO input pin
2. Build the project and download to the Starter Kit
3. LED should be blinking
4. Disconnect the periodic external signal
5. LED is now on and not blinking anymore, which indicates a WDOG timeout

Peripherals Used:
WDOG - 2 second period


Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PD02  - LED0
PA05  - GPIO PRS WDOG input (Expansion Header Pin 12)

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22A224F512IM40
PD02  - LED0
PA05  - GPIO PRS WDOG input (Expansion Header Pin 12)

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02  - LED0
PA05  - GPIO PRS WDOG input (Expansion Header Pin 7)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02  - LED0
PA05  - GPIO PRS WDOG input (Expansion Header Pin 7)

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06  - LED0
PA05  - GPIO PRS WDOG input (Expansion Header Pin 7)

Board:  Silicon Labs EFR32xG27 Buck Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00  - LED0
PA05  - GPIO PRS WDOG input (Expansion Header Pin 12)

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02  - LED0
PB04  - GPIO PRS WDOG input (Expansion Header Pin 11)
