
================================================================================

This project operates in EM3 and configures opamp 0 as a voltage follower, 
opamp 1 as a voltage follower, and opamp 2 as a non-inverting opamp to produce 
a differential amplifier with three opamps.

The equation for Vout is shown below (provided that R2/R1 for both opamps
are inverses of each other). They have to be inverses because the resistor
ladder being used for the input to the positive node of opamp 2 is actually
coming from opamp 0 and thus the two resistor ladders are not mirrors of each
other. Refer to the application note (AN0038) for an image illustrating this.

Vout2 = (Vin0 - Vin1) * (R2 / R1)
where Vin0 is the input to the positive node of opamp 0 and Vout2 is the output
of opamp 2

The generic equation for Vout is shown below:

Vout2 = (-Vin1 * (R2_OPA2 / R1_OPA2)) +
        (Vin0 * (R1_OPA0 / (R1_OPA0 + R2_OPA0)) * (1 + (R2_OPA2 / R1_OPA2)))

By default, this project selects the R2/R1 resistor ladder ratios for both
opamps to be inverses such that Vout2 = (Vin0 - Vin1) * 3.

Given the possible resistor values, where the R2/R1 ratios are inverses of each
other, there are three output gains available: 
Vout2 = 3 * (Vin0 - Vin1)
Vout2 = 1 * (Vin0 - Vin1)
Vout2 = 1/3 * (Vin0 - Vin1)

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

Note: This project does not work for the radio 14 boards (MG14, BG14, FG14)
because they only have 2 opamps and this project uses three opamps

Note: You might need to make the voltage sources and the oscilloscope share the
same ground.

================================================================================

Peripherals Used:
OPAMP
DAC

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use a DC power supply with two voltage sources and connect positive output #0
   to the positive input pin of opamp 0 (the OPAMP_P0 or OPA0_P pin).
3. Connect positive output #1 to the positive input pin of opamp 1
   (the OPAMP_P1 or OPA1_P pin).
4. Use an oscilloscope to measure the output of opamp 2 (OPAMP_OUT2 or OPA2_OUT)
5. If successful, the oscilloscope will show
   Vout2 = (Vin0 - Vin1) * (R2 / R1)
   By default, (R2 / R1) will be equal to 3

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PD6  - OPAMP_P1      (Expansion Header Pin 16)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PD6  - OPAMP_P1      (Expansion Header Pin 16)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PD6  - OPAMP_P1      (Expansion Header Pin 16)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PC4  - OPAMP_P0      (Expansion Header Pin 3)
PD6  - OPAMP_P1      (Expansion Header Pin 16)
PD5  - OPAMP_OUT2 #0 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 12)
PD11 - OPA1_P   (APORT4XCH3) (Expansion Header Pin 14)
PD9  - OPA2_OUT (APORT3YCH1) (Expansion Header Pin 5)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA1_P   (APORT4XCH3) (Expansion Header Pin 9)
PD9  - OPA2_OUT (APORT3YCH1) (Expansion Header Pin 5)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA2_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA1_P   (APORT4XCH3) (Expansion Header Pin 9)
PD9  - OPA2_OUT (APORT3YCH1) (Expansion Header Pin 5)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA2_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA1_P   (APORT4XCH3) (Expansion Header Pin 9)
PD9  - OPA2_OUT (APORT3YCH1) (Expansion Header Pin 5)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA2_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE10 - OPA0_P   (APORT3XCH10) (Expansion Header Pin 4)
PE11 - OPA1_P   (APORT4XCH11) (Expansion Header Pin 6)
PE12 - OPA2_OUT (APORT4YCH12) (Expansion Header Pin 8)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PA14 - OPA0_P   (APORT1XCH14) (Breakout Pads Pin 5)
PD6  - OPA1_P   (main input)  (Expansion Header Pin 16)
PD5  - OPA2_OUT (main output) (Expansion Header Pin 11)

