gpio_slew_rate

This project demonstrates the slew rate settings for the GPIO. A 1 MHz square
wave is generated on a pin. The slew rate setting is changed by pressing BTN0.
The drive strength setting is changed by pressing BTN0.  The effects of these
settings on the pin's slew rate can be observed by placing a 50uF capacitor on
the output pin and observing the change in rise and fall times of the 1 MHz
square wave.

In series 2, both of these are controlled with the slew rate setting. Slew rate
is a 3 bit setting. The lowest setting of 000 will result in low drive strength.
Any other value is understood as high drive strength.

Note for EFR32xG21 devices, clock enabling is not required.

How To Test:
1. Place a 50uF capacitor between the output pin and GND.
2. Upload and run the example.
3. While observing the rise and fall times of the waveform on the output pin, 
   press PB0 to change the slew rate.
    
Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz
EMU
TIMER  - FSRCO @ 20 MHz, Toggles GPIO at 1 MHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD02 -  Push Button 0
PC00 -  1 MHz output (Expansion Header Pin 4)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB00 -  Push Button 0
PC00 -  1 MHz output (Expansion Header Pin 4)

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  Push Button 0
PC00 -  1 MHz output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  Push Button PB0
PC00 -  1 MHz output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  Push Button PB0
PC00 -  1 MHz output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 -  Push Button PB0
PC03 -  1 MHz output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  Push Button PB0
PD10 -  1 MHz output (Expansion Header Pin 10)