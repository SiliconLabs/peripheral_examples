acmp_polled

This project demonstrates interrupt-driven use of the ACMP by comparing
the voltage on an input pin when a button is pressed to the internal
1.25 V reference (VREF).

When the button is pushed, the input voltage is pulled to GND, and the
comparator output goes low. Similarly, when the button is released, the
output is pulled to the common AVDD/IOVDD supply, and the comparator
output goes high.

During operation, the CPU constantly polls the state of the comparator
output (ACMP_STATUS_ACMPOUT bit) and when an input change is detected,
the GPIO pin connected to LED0 is driven low or high in response.  As
in the acmp_pin_output example, the comparator output is also directly
routed to a GPIO pin and can, if desired, be observed with a multimeter
or an oscilloscope.

How To Test:
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary).
2.  Build the example project and download it to the target system.
3.  Press and hold button PB0; LED0 will turn on.
4.  When button PB0 is released; LED0 will turn off.

How To Test (EFR32xG27):
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary).
2.  Build the example project and download it to the target system.
3.  LED0 is on by default. Press and hold button PB1; LED0 will turn off.
4.  When button PB1 is released; LED0 will turn on.

NOTE: Button PB0 provides an easy way to say the example in action, but
mechanical bouncing may cause extra rising or falling edges on the ACMP
input that cross the VREF threshold. Any of these can result in an
interrupt request that changes the state of the GPIO pin driving LED0.

Consider using an active device, such as a programmable power supply or
the output from another processor, to drive the ACMP input so that its
behavior can be observed in the absence of undamped mechanical noise.

Peripherals Used:
ACMP - Full input range from GND to VMCU (common AVDD/IOVDD supply)
       Low accuracy mode, less current usage
       Hysteresis disabled
       VREFDIV set to maximum to disable reference division

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PD02 -  ACMP positive input, push button PB0, Expansion Header Pin 7, WSTK Pin 4
PD03 -  ACMP output, Expansion Header Pin 9, WSTK Pin 6
PB01 -  LED1, Expansion Header Pin 13, WSTK Pin 10

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PB01 -  ACMP positive input, push button PB0
PB03 -  ACMP output, push button PB1
PB02 -  LED0, Expansion Header Pin 13, WSTK Pin 10

Board:  Silicon Labs EFR32MG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48
PB01 -  ACMP positive input, push button PB0, WSTK Pin 17
PB02 -  LED0, WSTK Pin 19

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
PB01 -  ACMP positive input, push button PB1, WSTK Pin 21
PC06 -  LED0, ACMP output, WSTK Pin 27

Board:  Silicon Labs EFR32xG27 2.4 GHz 8 dBm Buck DCDC Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
PB00 -  LED0, WSTK Pin 4, Expansion Header Pin 7
PB01 -  ACMP positive input, push button PB1, WSTK Pin 6, Expansion Header Pin 9
PB03 -  ACMP output, WSTK Pin 13, Expansion Header Pin 16

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio
        Board (BRD4400C) + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  ACMP positive input, push button PB0, WSTK Pin 17
PB02 -  LED0, WSTK Pin 19
PB03 -  ACMP output, push button PB1, WSTK Pin 21