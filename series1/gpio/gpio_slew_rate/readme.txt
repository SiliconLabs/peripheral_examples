gpio_slew_rate

This project demonstrates the slew rate settings for the GPIO. A 1 MHz square
wave is generated on a pin. The slew rate setting is changed by pressing PB0.
The drive strength setting is changed by pressing PB1. The effects of these
settings on the pin's slew rate can be observed by placing a 50uF capacitor on
the output pin and observing the change in rise and fall times of the 1 MHz
square wave.

How To Test:
1. Place a 50uF capacitor between the output pin and GND.
2. Upload and run the example.
3. The slew rate setting and drive strength settings are displayed on the kit's LCD.
4. While observing the rise and fall imes of the waveform on the output pin:
    a. Press PB0 to change the slew rate setting.
    b. Press PB1 to change the drive strength setting.

Peripherals Used:
HFRCO - 19 MHz

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC8 - Push Button PB0
PC9 - Push Button PB1
PD1 - 1 MHz output

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF6 - Push Button PB0
PF7 - Push Button PB1
PA0 - 1 MHz output

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF6 - Push Button PB0
PF7 - Push Button PB1
PA0 - 1 MHz output
