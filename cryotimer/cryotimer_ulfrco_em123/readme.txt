
================================================================================

This project shows how to use the Cryotimer with the ULFRCO in EM3. The project
idles in EM3 while waiting for the interrupt handler to toggle LED0. An
interrupt occurs every 2048 ULFRCO clock cycles (about 2.05 seconds) by
default.

This project can be changed to use the low-frequency crystal oscillator (LFXO)
or low-frequency RC oscillator (LFRCO) but must be limited to running in EM1 or
EM2 because these oscillators are disabled in EM3.

The duration between Cryotimer wakeup events is defined to be the following 
(in units of seconds):
  Time_Wakeup = (2 ^ PRESC) * (2 ^ PERIODSEL) / (F_CRYOCLK)
  F_CRYOCLK = 1000 Hz for ULFRCO
  F_CRYOCLK = 32768 Hz for LFXO and LFRCO

================================================================================

Peripherals Used:
ULFRCO - 1000 Hz
CRYOTIMER

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. LED0 will be on for 2 seconds and then off for 2 seconds. This cycle will
   repeat indefinitely.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF4 - LED0

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF4 - LED0

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PF4 - LED0

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PF4 - LED0

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PF4 - LED0

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PF4 - LED0

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PF4 - LED0

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PF4 - LED0

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PF4 - LED0

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PH10 - LED0 (Red)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - LED0

