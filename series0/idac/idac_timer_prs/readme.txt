
================================================================================

This example shows how to use a timer to control the IDAC through the PRS. The
IDAC is configured to enable its output based on the signal it receives from PRS
channel 0. The timer generates a square waveform signal that it sends to the
PRS. This project operates in EM1 even though the IDAC works in EM3 because the
timers are only capable of operating up to EM1. An example use case of this
project is PRS triggered charge injection to control current output.

Note: the following devices do not have an IDAC module
 - G (Gecko)
 - GG (Giant Gecko)
 - LG (Leopard Gecko)
 - WG (Wonder Gecko)
 - TG (Tiny Gecko)
 - TG11 (Tiny Gecko 11)

================================================================================

Peripherals Used:
 - IDAC
 - PRS
 - TIMER0
 - HFPERCLK

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use a breadboard to make a circuit that has the current output coming from
   the IDAC0_OUT pin going over an external resistor to ground. Also, make sure
   to select a resistor value such that V = I * R does not exceed the voltage
   supply rail (i.e. 3.3 Volts). For example, since the device is configured to
   output 64 microamps by default, one should choose a resistor that has less
   than 51.5 kOhms of resistance.
3. Measure the voltage drop across the resistor on an oscilloscope. If the
   max is equal to V = I * R and the min is equal to zero, then the output is
   correct.

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

