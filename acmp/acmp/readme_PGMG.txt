acmp

This project demonstrates a simple comparison of 2 inputs.
PB1 is an input, which is compared with internal Vref of 2.5V.
When the button is pressed output pin PA1 will be set high as
the voltage input will be greater than VREF, when it is low
PA1 will be low.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure the output of pin PA1 with PB1 pressed and not pressed

Peripherals Used:
HFRCO  - 19 MHz
ACMP0 


Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
Push Button 1(PF7)- ACMP0 input
PA1(Expansion header 14)- ACMP0 output

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
Push Button 1(PF7)- ACMP0 input
PA1(Expansion header 14)- ACMP0 output

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
Push Button 1(PF7)- ACMP0 input
PA1(Expansion header 14)- ACMP0 output

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
Push Button 1(PF7)- ACMP0 input
PA1(Expansion header 14)- ACMP0 output