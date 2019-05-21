dac_voltage

This project uses the DAC in continuous mode to output 0.5V to a pin in EM1. 
The DAC operates independently from the core. Since the DAC is operating in 
continuous mode, the output voltage will be stable even if the output is 
loaded. The fact that the DAC is continuously working will be reflected by the 
power consumption.

How To Test:
1. Build the project and download to the Starter Kit
2. Measure the DAC output pin

Peripherals Used:
HFRCO - 19 MHz
DAC  - internal 1.25V low noise reference, continuous mode

Board: Silicon Labs EFM32G Starter Kit (STKXXX)
Device: EFM32GG890F128
PB11 - DAC0_OUT0

Board: Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PB11 - DAC0_OUT0

Board: Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PB11 - DAC0_OUT0

Board: Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PB11 - DAC0_OUT0





