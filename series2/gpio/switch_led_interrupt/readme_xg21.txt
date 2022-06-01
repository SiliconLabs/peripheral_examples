switch_led_interrupt

This project demonstrates how to use GPIO pins to trigger external interrupts.
If a push-button is pressed, and it is on an odd pin, the odd GPIO interrupt is
triggered and toggles LED1. If the pin is on an even pin, the even GPIO
interrupt is triggered and toggles LED0.

Because EFR32MG21 can only wake from EM2/3 from GPIO interrupts on ports A or
B, push buttons on the WSTK (connected to port D) must be jumped to an
interrupt capable port. For this example, jump pins 7 and 9 to pins 14 and 12,
respectively, on the Expansion Header of the WSTK.

Note for EFR32MG21, clock enabling is not required.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect Push Button 0 to PA6 (pin 7 to 14)
3. Connect Push BUtton 1 to PA5 (pin 9 to 12)
4. Press PB0 to toggle LED0
5. Press PB1 to toggle LED1

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB0 - LED0
PB1 - LED1
PD2 - Push Button PB0, WSTK EXP Header Pin 7
PD3 - Push Button PB1, WSTK EXP Header Pin 9
PA5 - WSTK EXP Header Pin 12
PA6 - WSTK EXP Header Pin 14
