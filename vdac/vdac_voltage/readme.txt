vdac_voltage

This project uses the VDAC in continuous mode to output 0.5V to a pin in EM1. 
The VDAC operates independently from the core. Since the VDAC is operating in 
continuous mode, the output voltage will be stable even if the output is 
loaded. The fact that the DAC is continuously working will be reflected by the 
power consumption.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure the VDAC output pin

Peripherals Used:
HFRCO - 19 MHz
VDAC  - internal 1.25V low noise reference, continuous mode

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA3 - VDAC0_OUT0

Board:  Silicon Labs EFR32MG12 Radio Board (SLWRB4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
PA3 (P36) - VDAC0_OUT0

Board:  Silicon Labs FR32BG12 Radio Board (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG12P332F1024GL125
PA3 (P36) - VDAC0_OUT0

Board:  Silicon Labs EFR32FG12 Radio Board (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG12P433F1024GL125
PA3 (P36) - VDAC0_OUT0






