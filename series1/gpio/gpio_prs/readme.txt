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
HFRCO - 19 MHz

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A) +
        Wireless Starter Kit Mainboard
Device: EFM32BG1P232F256GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A) +
        Wireless Starter Kit Mainboard
Device: EFM32BG12P332F1024GL125
PF6 - Push Button PB0

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A) +
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A) +
        Wireless Starter Kit Mainboard
Device: EFM32FG1P133F256GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A) +
        Wireless Starter Kit Mainboard
Device: EFM32FG12P433F1024GL125
PF6 - Push Button PB0

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A) +
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFR32FG14 Starter Kit(BRD4257A) +
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A) +
        Wireless Starter Kit Mainboard
Device: EFM32MG1P232F256GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFM32MG12P Starter Kit (BRD4161A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
PF6 - Push Button PB0

Board:  Silicon Labs EFR32MG13 Starter Kit (BRD4159A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B) +
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD5 - Push Button PB0

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC8 - Push Button PB0

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF6 - Push Button PB0

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF6 - Push Button PB0
