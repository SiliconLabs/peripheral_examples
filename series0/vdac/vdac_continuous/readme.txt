
================================================================================

This project uses the VDAC in continuous mode with single-ended output to output
0.5V to a pin in EM3. The VDAC operates independently from the core. Since the
VDAC is operating in continuous mode, the output voltage will be stable even if
the output is loaded. The fact that the VDAC is continuously working will be
reflected by the power consumption.

Note: all of the boards except the EFM32G (GXXX-STK) (Gecko) can use the
DAC/VDAC in EM3. EFM32G can use the DAC in EM1.

This example used about 270 microamps on average. After commenting out the line
of code that puts the board in EM2, this example used about 1.91 milliamps on
average. Note: this energy measurement was done using Simplicity Studio's
built-in energy profiler for a GG11 board with a Debug build configuration and no
optimization flags (gcc -O0).

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Measure the VDAC output pin with respect to ground

================================================================================

Peripherals Used:
HFRCO - 19 MHz
VDAC  - internal 1.25V low noise reference, continuous mode

================================================================================

Listed below are the boards that do not have a VDAC module
 - Zero Gecko (ZG)
 - Happy Gecko (HG)
 - Pearl Gecko 1 (PG1)
 - Mighty Gecko 1 (MG1)
 - Blue Gecko 1 (BG1)
 - Flex Gecko 1 (FG1)

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32G Starter Kit (GXXX-STK)
Device: EFM32G890F128
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA3 - VDAC0_OUT0 (Pin 11 of breakout pads)

Board:  Silicon Labs EFR32MG12 Radio Board (SLWRB4161A)
Device: EFR32MG12P432F1024GL125
PA3 - VDAC0_OUT0 (Pin 36 of breakout pads)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PA3 - VDAC0_OUT0 (Pin 2 of breakout pads)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PA3 - VDAC0_OUT0 (Pin 2 of breakout pads)

Board:  Silicon Labs FR32BG12 Radio Board (BRD4103A)
Device: EFR32BG12P332F1024GL125
PA3 - VDAC0_OUT0 (Pin 36 of breakout pads)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PA3 - VDAC0_OUT0 (Pin 2 of breakout pads)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PA3 - VDAC0_OUT0 (Pin 2 of breakout pads)

Board:  Silicon Labs EFR32FG12 Radio Board (BRD4253A)
Device: EFR32FG12P433F1024GL125
PA3 - VDAC0_OUT0 (Pin 36 of breakout pads)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PA3 - VDAC0_OUT0 (Pin 2 of breakout pads)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PA3 - VDAC0_OUT0 (Pin 2 of breakout pads)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC0 - VDAC0_OUT0 (Pin 3 of expansion headers)

