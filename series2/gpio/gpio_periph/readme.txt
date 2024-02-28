gpio_periph

This project shows how to drive a pin the output of an on-chip oscillator
(the LFRCO, in this case) and how to set the default and alternate slew
rates for the selected GPIO port.

Note: Oscillators are enabled on-demand on Series 2 devices.  Exporting
the output of an oscillator to a pin is such an on-demend request and
enables the selected oscillator.  There is no need to do so using
CMU_OscillatorEnable() as is the case on Series 0 and 1 devices.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect an oscilloscope to pin PC3 and observe the LFRCO signal

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz, LFRCO @ 32.768 kHz

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PC03 -  LFRCO output (Expansion Header Pin 10)

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22C224F512IM40
PC03 -  LFRCO output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PC03 -  LFRCO output (Expansion Header Pin 8)

Board:  Silicon Labs EFR32MG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48
PC03 -  LFRCO output (Expansion Header Pin 8)

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
PC02 -  LFRCO output (Expansion Header Pin 8)

Board:  Silicon Labs EFR32xG27 2.4 GHz 8 dBm Buck DCDC Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
PC03 -  LFRCO output (Expansion Header Pin 10)

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio
        Board (BRD4400C) + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PD09 -  LFRCO output (Expansion Header Pin 8)