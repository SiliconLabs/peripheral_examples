
================================================================================

This example uses the IDAC to output 2 microamps to the pin specified in the pin
mappings section below. The project operates in EM3. This example used about
21.2 microamps when in EM3. After commenting out the line of code that puts the
board in EM3, this example used about 1.64 milliamps on average. Note: this
energy measurement was done using Simplicity Studio's built-in energy profiler
for the GG11 board with a Debug build configuration and no optimization flags
(gcc -O0).

Note: the following boards do not have an IDAC module
 - G (Gecko)
 - GG (Giant Gecko)
 - LG (Leopard Gecko)
 - WG (Wonder Gecko)
 - TG (Tiny Gecko)
 - TG11 (Tiny Gecko 11)

================================================================================

Peripherals Used:
IDAC

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit.
2. Use a multimeter to measure the current on the output pin specified below

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PB11 - IDAC0_OUT (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PB11 - IDAC0_OUT (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PB8  - IDAC0_OUT (pin 11 of expansion headers)

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A)
Device: EFM32MG1P232F256GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PD12 - IDAC0_OUT (pin 11 of expansion headers)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A)
Device: EFM32FG1P133F256GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PD12 - IDAC0_OUT (pin 11 of expansion headers)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A)
Device: EFM32BG1P232F256GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PD12 - IDAC0_OUT (pin 11 of expansion headers)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PA2  - IDAC0_OUT (pin 3 of expansion headers)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE10 - IDAC0_OUT (pin 4 of expansion headers)

