gpio_slew_rate

This project demonstrates the slew rate settings for the GPIO. A 1 MHz square
wave is generated on a pin. The slew rate setting is changed by pressing BTN0.
The drive strength setting is changed by pressing BTN1.  The effects of these
settings on the pin's slew rate can be observed by placing a 50uF capacitor on
the output pin and observing the change in rise and fall times of the 1 MHz
square wave.

In series 2, both of these are controlled with the slew rate setting. Slew rate
is a 3 bit setting. The lowest setting of 000 will result in low drive strength.
Any other value is understood as high drive strength.

How To Test:
1. Place a 50uF capacitor between the output pin and GND.
2. Upload and run the example.
3. The slew rate setting and drive strength settings are displayed on the kit's LCD.
4. While observing the rise and fall imes of the waveform on the output pin, press
   PB0 to change the slew rate.
    
Peripherals Used:
FSRCO  - 20 MHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD2 - Push Button PB0
PC0 - 1 MHz output

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB0 - Push Button PB0
PC0 - 1 MHz output
