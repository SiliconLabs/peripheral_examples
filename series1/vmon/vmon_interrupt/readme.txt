VMON_Interrupt

This project demonstrates the VMON to detect battery level threasholds
in EM2.  When the source battery is below the THRESHOLD_VOLTAGE (3.0 V),
LED0 is off.  When the battery is above THRESHOLD_VOLTAGE, LED0 is on.
Interrupts occur on rising and falling edges to re-evaluate the value 
of LED0.

This project currently works off of AVDD.  To switch to a different
channel, change VMON_CHANNEL and VMON_IF macros by replacing all
instances of AVDD with your desired channel.


How To Test:
1. Build the project and download to the Starter Kit
2. Switch the kit to battery mode
3. The state of LED0 should reflect that battery voltage

Peripherals Used:
HFRCO  - 19 MHz
VCMP


Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32BG13 Radio Board (SLWRB4104A)
Device: EFR32BG13P632F512GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32BG14 Radio Board (SLWRB4105A)
Device: EFR32BG14P732F256GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32FG13 Radio Board (SLWRB4256A)
Device: EFR32FG13P233F512GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32FG14 Radio Board (SLWRB4257A)
Device: EFR32FG14P233F256GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32MG13 Radio Board (SLWRB4159A)
Device: EFR32MG13P632F512GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFR32MG14 Radio Board (SLWRB4169A)
Device: EFR32MG14P733F256GM48
HFRCO  - 19 MHz
PF4 - LED0

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
HFRCO  - 19 MHz
PH10 - LED0

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
HFRCO  - 19 MHz
PD2 - LED0

