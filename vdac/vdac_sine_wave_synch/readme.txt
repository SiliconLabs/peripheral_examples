
================================================================================

This project uses the DAC and the internal sine wave generator to produce a
16-point sine wave at a frequency of f_sinewave Hz centered at the reference
voltage divided by 2. This project operates in EM1. By default, this program
outputs a sinewave at 27.343 kHz for series 0 boards and 31.25 kHz for series 1
boards.

Note that the DAC_CLK can only be programmed to a maximum of 1 MHz.
In this example, f_IN_CLK is the HFPERCLK.
Which is (14 MHz for series 0 boards) (19 MHz for series 1 boards).
For series 0 boards: f_DAC_CLK = f_IN_CLK / (2 ^ PRESCALE)
For series 1 boards: f_DAC_CLK = f_IN_CLK / (PRESCALE + 1)

These equations below show how the calculation for the sinewave frequency.
For series 0 boards: f_sinewave = f_HFPERCLK / (32 * (2 ^ PRESCALE)).
For series 1 boards: f_sinewave = f_HFPERCLK / (32 * (PRESCALE + 1))

The PRESCALE bitfield is different for series 0 and series 1 boards. For series
0 boards, the bitfield is 3 bits so the actual prescaler can be any power of 2
between 1 and 128. For series 1 boards, the bitfield is 7 bits so the actual
prescaler can be any number between 1 and 128. The prescaler must be chosen, as
mentioned above, to ensure that the DAC_CLK is less than or equal to 1 MHz.

Although the DAC can operate in EM3 for all series 0 and series 1 boards (except
the original Gecko), synchronous reflexes are clocked on the HFPERCLK so they do
not work in EM2/3 (high frequency clocks get turned off in EM2/3). Furthermore,
the DAC in series 0 boards does not support asynchronous mode so EM1 is the
lowest energy mode possible for a project that uses the DAC and the PRS (such as
this example).

The PRS channel that inputs to DAC/VDAC channel 0 controls the sinewave enable.
The PRS channel that inputs to DAC/VDAC channel 1 controls the output enable.
Both need to be high to see the sinewave output.
The sine wave is output on DAC/VDAC channel 0.

This example used about 1.4 milliamps on average. After commenting out the line
of code that puts the board in EM1, this example used about 2.10 milliamps on
average. Note: this energy measurement was done using Simplicity Studio's
built-in energy profiler for a GG11 board with a Debug build configuration and no
optimization flags (gcc -O0).

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Use an oscilloscope to measure the output pin.

================================================================================

Peripherals Used:
HFPERCLK - 19 MHz (series 1 boards) or 14 MHz (series 0 boards)
VDAC - internal 1.25V low noise reference, sine wave mode
PRS

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
PC0 - VDAC0_OUT0_ALT (Pin 3 of expansion headers)

