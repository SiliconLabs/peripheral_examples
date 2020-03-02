gpio_prs

This project demonstrates how the GPIO triggers an ADC scan via PRS.  The
device runs the device in EM1. A falling edge (pushing button PB0) triggers a
single ADC conversion. When the conversion ends, the ADC requests an interrupt
to wake up the processor, and a delay cycle keeps it running in EM0 for 1
second before re-entering EM1. This allows the change in current draw to be
observed using the Advanced Energy Monitor or the EnergyAware Profiler.

How To Test:
1. Build the project and download to the Starter Kit
2. Open Energy Profiler in Simplicity Studio
3. Press PB0 and observe increased current consumption for 1 second

Peripherals Used:
FSRCO  - 20 MHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD2 - Push Button PB0
PC3 - IADC input

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB0 - Push Button PB0
PC3 - IADC input
