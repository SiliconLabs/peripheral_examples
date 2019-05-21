
================================================================================

This project configures opamp 2 as a non-inverting amplifier whose gain is given
by the following equation: Vout = Vin * (1 + R2/R1). By default, this project
selects the R2/R1 resistor ladder ratio to be R2 = R1. This results in
Vout = Vin * 2. This project also configures the DAC to output on Channel 0. The
user should use a wire to connect the output of the DAC to the positive input
node of the opamp.

Note: An opamp that is being used by the DAC/VDAC cannot be used as a standalone
opamp at the same time. Opamps 0 and 1 are contained within the DAC/VDAC module
and correspond to the DAC/VDAC channel 0 and 1. Meaning, if DAC/VDAC channel 0
is being used then opamp 0 cannot be used at the same time.

This project uses the APORT to route signals to the opamp's input nodes
and to route the opamp's output to a GPIO pin. The APORT allows for flexible
configuration on series 1 boards. For series 0 boards, the routing is much more
limited (in most cases only 1 pin is available for each opamp input and output).
As a result, the main output is used for series 0 boards. Additionally, the TG11
uses opamp 2's main output in conjunction with the APORT for routing the
input to the positive terminal of the opamp (this is because the APORT, despite
being very flexible, did not coincide with any of the pins brought out to the
starter kit).

Note: To change the pin mappings for the series 1 projects, refer to the APORT
section of your board's datasheet and change the mappings accordingly by using
EMLIB type defines. For series 0 boards, consider using the alternate outputs
instead (note though that the alternate outputs have a lower drive strength).

Note: This project cannot be run on the TG11 starter kit board because the
VDAC0_OUT0 pin is not brought out to the board.

================================================================================

Peripherals Used:
OPAMP
DAC

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use a wire to connect the DAC/VDAC opamp output pin to the positive input pin
   of the opamp.
3. Use an oscilloscope to check the opamp output pin.
4. If successful, the oscilloscope will show that the output is double the
   DAC/VDAC output.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD4  - OPAMP_P2      (Expansion Header Pin 12)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)
PB11 - VDAC0_OUT0    (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD4  - OPAMP_P2      (Expansion Header Pin 12)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)
PB11 - VDAC0_OUT0    (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD4  - OPAMP_P2      (Expansion Header Pin 12)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)
PB11 - VDAC0_OUT0    (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PD4  - OPAMP_P2      (Expansion Header Pin 12)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)
PB11 - VDAC0_OUT0    (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PD10 - OPA2_P   (APORT3XCH2) (Expansion Header Pin 12)
PD11 - OPA2_OUT (APORT3YCH3) (Expansion Header Pin 14)
PA3  - VDAC0_OUT0            (Pin 11 of breakout pads)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PD10 - OPA2_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA2_OUT (APORT3YCH3) (Expansion Header Pin 9)
PA3  - VDAC0_OUT0            (Pin 36 of breakout pads)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PC6 - OPA2_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA2_OUT (APORT1YCH7) (Expansion Header Pin 6)
PA3 - VDAC0_OUT0            (Pin 2 of breakout pads)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PC6 - OPA1_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_OUT (APORT1YCH7) (Expansion Header Pin 6)
PA3 - VDAC0_OUT0            (Pin 2 of breakout pads)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PD10 - OPA2_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA2_OUT (APORT3YCH3) (Expansion Header Pin 9)
PA3  - VDAC0_OUT0            (Pin 36 of breakout pads)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC6 - OPA2_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA2_OUT (APORT1YCH7) (Expansion Header Pin 6)
PA3 - VDAC0_OUT0            (Pin 2 of breakout pads)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PC6 - OPA1_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_OUT (APORT1YCH7) (Expansion Header Pin 6)
PA3 - VDAC0_OUT0            (Pin 2 of breakout pads)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PD10 - OPA2_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA2_OUT (APORT3YCH3) (Expansion Header Pin 9)
PA3  - VDAC0_OUT0            (Pin 36 of breakout pads)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC6 - OPA2_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA2_OUT (APORT1YCH7) (Expansion Header Pin 6)
PA3 - VDAC0_OUT0            (Pin 2 of breakout pads)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PC6 - OPA1_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_OUT (APORT1YCH7) (Expansion Header Pin 6)
PA3 - VDAC0_OUT0            (Pin 2 of breakout pads)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE10 - OPA2_P   (APORT3XCH10) (Expansion Header Pin 4)
PE11 - OPA2_OUT (APORT3YCH11) (Expansion Header Pin 6)
PB11 - VDAC0_OUT0             (Pin 11 of expansion headers)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD6  - OPA1_P   (Positive Pad) (Expansion Header Pin 16)
PA14 - OPA1_OUT (APORT2YCH14)  (Breakout Pads Pin 5)
PB11 - VDAC0_OUT0              (Pin 3 of expansion headers)

