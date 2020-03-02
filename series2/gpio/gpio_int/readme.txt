gpio_int

This project demonstrates how to use GPIO pins to trigger external interrupts.
If a push-button is pressed, and it is on an odd pin, the odd GPIO interrupt is
triggered and toggles LED1. If the pin is on an even pin, the even GPIO
interrupt is triggered and toggles LED0.

How To Test:
1. Build the project and download to the Starter Kit
2. Press PB0 to toggle LED0
3. Press PB1 to toggle LED1

Peripherals Used:
FSRCO  - 20 MHz

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PD2 - LED0
PD3 - LED1
PB0 - Push Button PB0
PB1 - Push Button PB1
