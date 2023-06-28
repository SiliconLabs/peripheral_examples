gpio_periph

This project demonstrates how to configure a GPIO pin to drive the output of
the LFRCO, and sets the slew rate and drive strength.

Note for EFR32xG21 devices, clock enabling is not required.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect an oscilloscope to pin PC3 and observe the LFRCO signal

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz, LFRCO @ 32.768 kHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PC03 -  LFRCO output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PC03 -  LFRCO output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PC03 -  LFRCO output (Expansion Header Pin 8)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PC03 -  LFRCO output (Expansion Header Pin 8)

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC02 -  LFRCO output (Expansion Header Pin 8)

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PC03 -  LFRCO output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PD09 -  LFRCO output (Expansion Header Pin 8)