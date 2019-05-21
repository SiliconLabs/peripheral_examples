
================================================================================

This project operates in EM3 and configures opamp 0, 1, 2 as cascading 
non-inverting amplifiers gains are given by the following equations:

Vout0 = Vin0  * (1 + R2/R1)
Vout1 = Vout0 * (1 + R2/R1) = Vin0 * ((1 + R2/R1) ^ 2)
Vout2 = Vout1 * (1 + R2/R1) = Vin0 * ((1 + R2/R1) ^ 3)

By default, this project selects the R2/R1 resistor
ladder ratio to be R2 = R1. This results in

Vout0 = Vin0 * 2
Vout1 = Vout0 * 2 = Vin0 * 4
Vout2 = Vout1 * 2 = Vin0 * 8

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

Note: The radio14 boards (MG14, BG14, FG14) only have 2 opamps so their cascaded
output equation is different from the other projects (i.e. Vout1 is the final
output instead of Vout2).

Note: This project uses the same resistor ratio (R2 = R1) for all of the opamps,
but the resistor ratio can be individually changed for each opamp.

================================================================================

Peripherals Used:
OPAMP
DAC

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use a DC power supply and connect the positive output to the positive
   input pin of opamp 0 (the OPAMP_P0 or OPA0_P pin)
3. Use an oscilloscope to measure the output pin of opamp 2
   (the OPAMP_OUT2 or OPA2_OUT pin)
4. If successful, the opamp's output voltage will be 8 times the input voltage
   of the power supply.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PB11 - OPAMP_OUT0 #0 (Expansion Header Pin 11)
PB12 - OPAMP_OUT1 #0 (Expansion Header Pin 13)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PB11 - OPAMP_OUT0 #0 (Expansion Header Pin 11)
PB12 - OPAMP_OUT1 #0 (Expansion Header Pin 13)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PB11 - OPAMP_OUT0 #0 (Expansion Header Pin 11)
PB12 - OPAMP_OUT1 #0 (Expansion Header Pin 13)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PC4  - OPAMP_P0      (Expansion Header Pin 3)
PB11 - OPAMP_OUT0 #0 (Expansion Header Pin 11)
PB12 - OPAMP_OUT1 #0 (Expansion Header Pin 13)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 12)
PA3  - OPA0_OUT              (Breakout Pads Pin 11)
PD14 - OPA1_OUT              (Breakout Pads Pin 14)
PD11 - OPA2_OUT (APORT3YCH3) (Expansion Header Pin 14)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PA3  - OPA0_OUT              (Breakout Pads Pin 36)
PD14 - OPA1_OUT              (Not brought out to a pin)
PD11 - OPA2_OUT (APORT3YCH3) (Expansion Header Pin 9)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PC6  - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PA3  - OPA0_OUT              (Expansion Header Pin 5)
PD14 - OPA1_OUT              (Breakout Pads Pin 33)
PC7  - OPA2_OUT (APORT1YCH7) (Expansion Header Pin 6)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PC6  - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PA3  - OPA0_OUT              (Expansion Header Pin 5)
PC7  - OPA1_OUT (APORT1YCH7) (Expansion Header Pin 6)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PA3  - OPA0_OUT              (Breakout Pads Pin 36)
PD14 - OPA1_OUT              (Not brought out to a pin)
PD11 - OPA2_OUT (APORT3YCH3) (Expansion Header Pin 9)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC6  - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PA3  - OPA0_OUT              (Expansion Header Pin 5)
PD14 - OPA1_OUT              (Breakout Pads Pin 33)
PC7  - OPA2_OUT (APORT1YCH7) (Expansion Header Pin 6)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PC6  - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PA3  - OPA0_OUT              (Expansion Header Pin 5)
PC7  - OPA1_OUT (APORT1YCH7) (Expansion Header Pin 6)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PA3  - OPA0_OUT              (Breakout Pads Pin 36)
PD14 - OPA1_OUT              (Not brought out to a pin)
PD11 - OPA2_OUT (APORT3YCH3) (Expansion Header Pin 9)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC6  - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PA3  - OPA0_OUT              (Expansion Header Pin 5)
PD14 - OPA1_OUT              (Breakout Pads Pin 33)
PC7  - OPA2_OUT (APORT1YCH7) (Expansion Header Pin 6)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PC6  - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PA3  - OPA0_OUT              (Expansion Header Pin 5)
PC7  - OPA1_OUT (APORT1YCH7) (Expansion Header Pin 6)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE10 - OPA0_P   (APORT3XCH10) (Expansion Header Pin 4)
PB11 - OPA0_OUT               (Expansion Header Pin 11)
PB12 - OPA1_OUT               (Not brought out to a pin)
PE11 - OPA2_OUT (APORT3YCH11) (Expansion Header Pin 6)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PA14 - OPA0_P   (APORT1XCH14) (Breakout Pads Pin 5)
PB11 - OPA0_OUT               (Not brought out to a pin)
PB12 - OPA1_OUT               (Not brought out to a pin)
PD5  - OPA2_OUT (main output) (Expansion Header Pin 11)

