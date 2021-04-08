vdac_differential

This project uses the VDAC in continuous mode with differential output to output
a difference of 0.5V between two pins in EM3. The VDAC operates independently
from the core. Since the VDAC is operating in continuous mode, the output
voltage will be stable even if the output is loaded. The fact that the VDAC is
continuously working will be reflected by the power consumption.

Note: the VDAC/DAC uses the channel 0 data register as the input source and
sends the negative/positive output to the channel 0/1 output pins,
respectively.

Note: all of the boards except the EFM32G (GXXX-STK) (Gecko) can use the
DAC/VDAC in EM3. EFM32G can use the DAC in EM1.

This example used about 407 microamps on average. After commenting out the line
of code that puts the board in EM3, this example used about 2 milliamps on
average. Note: this energy measurement was done using Simplicity Studio's
built-in energy profiler for a GG11 board with a Debug build configuration and no
optimization flags (gcc -O0).

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Measure the VDAC output pins with respect to each other

================================================================================

Peripherals Used:
HFRCO - 14 MHz
VDAC  - internal 1.25V low noise reference, continuous mode

================================================================================

Listed below are the boards that do not have a VDAC module
 - Zero Gecko (ZG)
 - Happy Gecko (HG)

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32G Starter Kit (GXXX-STK)
Device: EFM32G890F128
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)
PB12 - VDAC0_OUT1 (Pin 13 of expansion headers)

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)
PB12 - VDAC0_OUT1 (Pin 13 of expansion headers)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)
PB12 - VDAC0_OUT1 (Pin 13 of expansion headers)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)
PB12 - VDAC0_OUT1 (Pin 13 of expansion headers)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PB11 - VDAC0_OUT0 (Pin 11 of expansion headers)
PB12 - VDAC0_OUT1 (Pin 13 of expansion headers)
