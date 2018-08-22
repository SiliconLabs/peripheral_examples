
================================================================================

This project operates in EM3 and configures opamp 0 as a voltage follower and
opamp 1 as a non-inverting opamp. The equation for Vdiff is shown below:

Vdiff = (Vin1 - Vin0) * (R2 / R1)
Vdiff = Vout1 - Vin1
where Vin0 is the input to the positive node of opamp 0 and
Vout1 is the output of opamp 1

By default, this project selects the R2/R1 resistor
ladder ratio to be R2 = 3 * R1. This results in
Vdiff = (Vin1 - Vin0) * 3

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

Note: the project for TG11 uses OPA1 and OPA2 because the pin mappings for OPA0
and OPA1 were readily available on the starter kit board.

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
4. Use an oscilloscope to measure the output of opamp 1 (OPAMP_OUT1 or OPA1_OUT)
5. If successful, the oscilloscope will show
   Vout1 = Vdiff + Vin1
         = (Vin1 - Vin0) * (R2 / R1) + Vin1

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PD6  - OPAMP_P1      (Expansion Header Pin 16)
PB12 - OPAMP_OUT1 #0 (Expansion Header Pin 13)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PD6  - OPAMP_P1      (Expansion Header Pin 16)
PB12 - OPAMP_OUT1 #0 (Expansion Header Pin 13)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PC4  - OPAMP_P0      (Expansion Header Pin 7)
PD6  - OPAMP_P1      (Expansion Header Pin 16)
PB12 - OPAMP_OUT1 #0 (Expansion Header Pin 13)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PC4  - OPAMP_P0      (Expansion Header Pin 3)
PD6  - OPAMP_P1      (Expansion Header Pin 16)
PB12 - OPAMP_OUT1 #0 (Expansion Header Pin 13)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 12)
PD11 - OPA1_P   (APORT4XCH3) (Expansion Header Pin 14)
PD9  - OPA1_OUT (APORT3YCH1) (Expansion Header Pin 5)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA1_P   (APORT4XCH3) (Expansion Header Pin 9)
PD9  - OPA1_OUT (APORT3YCH1) (Expansion Header Pin 5)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA1_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA1_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA1_P   (APORT4XCH3) (Expansion Header Pin 9)
PD9  - OPA1_OUT (APORT3YCH1) (Expansion Header Pin 5)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA1_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA1_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PD10 - OPA0_P   (APORT3XCH2) (Expansion Header Pin 7)
PD11 - OPA1_P   (APORT4XCH3) (Expansion Header Pin 9)
PD9  - OPA1_OUT (APORT3YCH1) (Expansion Header Pin 5)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA1_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PC6 - OPA0_P   (APORT1XCH6) (Expansion Header Pin 4)
PC7 - OPA1_P   (APORT2XCH7) (Expansion Header Pin 6)
PC8 - OPA1_OUT (APORT2YCH8) (Expansion Header Pin 8)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE10 - OPA0_P   (APORT3XCH10) (Expansion Header Pin 4)
PE11 - OPA1_P   (APORT4XCH11) (Expansion Header Pin 6)
PE12 - OPA1_OUT (APORT4YCH12) (Expansion Header Pin 8)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD6  - OPA1_P   (main input)  (Expansion Header Pin 16)
PA14 - OPA2_P   (APORT1XCH14) (Breakout Pads Pin 5)
PD5  - OPA2_OUT (main output) (Expansion Header Pin 11)

