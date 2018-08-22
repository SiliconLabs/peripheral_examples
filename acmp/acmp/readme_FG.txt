acmp

This project demonstrates a simple comparison of 2 inputs.
PB1 is an input, which is compared with internal Vref of 2.5V.
When the button is pressed output pin PA1 will be set high as
the voltage input will be greater than VREF, when it is low
PA1 will be low.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure the output of pin PA1 with PB0 pressed and not pressed

Peripherals Used:
HFRCO  - 19 MHz
ACMP0

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
Push Button 1(PF7)- ACMP0 input
PA1(Expansion header 14)- ACMP1 output

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
Push Button 1(PF7)- ACMP0 input
PA1(Expansion header 14)- ACMP1 output
