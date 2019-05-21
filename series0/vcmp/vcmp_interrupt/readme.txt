VCMP_Interrupt

This project demonstrates the VCMP to detect battery level thresholds
in EM2.  When the source battery is below the MAX_VOLTAGE (3.0 V), LED0
is off.  When the battery is above MAX_VOLTAGE, LED0 is on.  Interrupts
occur on falling edges to re-evaluate what value LED0 should have.


How To Test:
1. Build the project and download to the Starter Kit
2. Switch the kit to battery mode
3. Connect an external voltage source to the kit(just connect high voltage
to the battery port and connect the grounds). The state of LED0 should 
reflect if the battery voltage is above or below 3.0 V.

Peripherals Used:
HFRCO  - 19 MHz
VCMP


Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
HFRCO  - 14 MHz
PC0 - LED0

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
HFRCO  - 14 MHz
PE2 - LED0

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
HFRCO  - 14 MHz
PF4 - LED0

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
HFRCO  - 14 MHz
PE2 - LED0

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
HFRCO  - 14 MHz
PD7 - LED0

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
HFRCO  - 14 MHz
PE2 - LED0

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
HFRCO  - 14 MHz
PC10 - LED0
