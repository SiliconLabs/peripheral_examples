acmp

This project demonstrates a simple comparison of 2 inputs.
PB0 is an input, which is compared with internal Vref of 2.5V.
When the button is pressed output pin PA1 will be set high as
the voltage input will be greater than VREF, when it is low
PA1 will be low.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure the output of pin PA1 with PB0 pressed and not pressed

Peripherals Used:
HFRCO  - 19 MHz
ACMP1

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
Push Button 0(PF6)- ACMP1 input
PA1(Expansion header 14)- ACMP1 output

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
Push Button 0(PF6)- ACMP1 input
PA1(Expansion header 14)- ACMP1 output