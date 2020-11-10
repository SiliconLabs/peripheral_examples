wdog_led_toggle

This project demonstrates the functionality of the watchdog timer. It uses 
LED0 and a Push-button to indicate the state of the system. If LED0 is 
flashing, the system has not undergone a reset. If the Push Button 0 is 
pressed and held for some time, it will trigger a WDOG reset. The code looks
for a reset source in the beginning of the execution and if WDOG is found to 
be the reset source, LED0 is turned on and the code does not execute 
any further. 

How To Test:
1. Build the project and download to the Starter Kit
2. LED0 should be blinking
3. Press and hold Push Button 0 for a period of 3 seconds
4. LED0 is now ON and not blinking anymore, which is indicative of
   a WDOG reset

Peripherals Used:
WDOG - 2 seconds period


Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
       
Device: EFR32MG21A010F1024IM32
PD2 - LED0
PB0 - Push Button PB0

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG22A224F512IM40
PD2 - LED0
PB0 - Push Button PB0