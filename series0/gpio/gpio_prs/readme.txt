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
HFRCO - 14 MHz

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PB9 - Push Button PB0

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PB9 - Push Button PB0

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PC9 - Push Button PB0

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PB9 - Push Button PB0

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD8 - Push Button PB0

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PB9 - Push Button PB0

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC8 - Push Button PB0
