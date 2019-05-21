
================================================================================

This project operates in EM3 and configures opamps 0, 1, 2 as cascaded inverting
amplifiers whose gains are given by the following equations:

Vout0 = -(Vin - POS0)   * (R2 / R1) + POS0
Vout1 = -(Vout0 - POS1) * (R2 / R1) + POS1
Vout2 = -(Vout1 - POS2) * (R2 / R1) + POS2

Where POS is the input to the positive terminal of the opamp and Vin is the
input to the negative terminal of the opamp. POS usually equals (VS+ + VS-)/2,
giving full range on the output. For the EFM32 opamps this equals VDD/2.

By default, this project selects the R2/R1 resistor ladder ratio to
be R2 = R1.

This project uses the APORT to route signals to the opamp's input nodes
and to route the opamp's output to a GPIO pin. The APORT allows for flexible
configuration on series 1 boards. For series 0 boards, the routing is much more
limited (in most cases only 1 pin is available for each opamp input and output).
As a result, the main output is used for series 0 boards.

Note: To change the pin mappings for the series 1 projects, refer to the APORT
section of your board's datasheet and change the mappings accordingly by using
EMLIB type defines. For series 0 boards, consider using the alternate outputs
instead (note though that the alternate outputs have a lower drive strength).

Note: The TG11 project should not be run in the debugger because the project
default settings use the DEBUG pins. The reason this is the default project
setting for TG11 is because there were no other pin mappings possible to make
this project work on the starter kit

================================================================================

Peripherals Used:
OPAMP
DAC

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use a DC power supply with four voltage sources and connect positive 
   output #0 to the positive input pin of the opamp (POS0). Connect the other
   outputs to the POS1 and POS2 pins.
3. Connect another output to the negative input pin of the opamp (Vin).
4. Use an oscilloscope to measure the opamp output pin
5. If successful, the following equations will hold:
   Vout0 = -(Vin - POS0)   * (R2 / R1) + POS0
   Vout1 = -(Vout0 - POS1) * (R2 / R1) + POS1
   Vout2 = -(Vout1 - POS2) * (R2 / R1) + POS2
6. Alternatively, you could check that Vout2 is the following:
   Vout2 = (-Vin + POS0) * ((R2 / R1) ^ 3)
           + (POS0 - POS1) * ((R2 / R1) ^ 2)
           + (POS2 - POS1) * ((R2 / R1) ^ 1)
           + POS2

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PC4 - OPAMP_P0      (POS0)  (Expansion Header Pin 7)
PD6 - OPAMP_P1      (POS1)  (Expansion Header Pin 16)
PD4 - OPAMP_P2      (POS2)  (Expansion Header Pin 12)
PC5 - OPAMP_N0      (Vin)   (Expansion Header Pin 9)
PD5 - OPAMP_OUT2 #0 (Vout2) (Expansion Header Pin 14)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PC4 - OPAMP_P0      (POS0)  (Expansion Header Pin 7)
PD6 - OPAMP_P1      (POS1)  (Expansion Header Pin 16)
PD4 - OPAMP_P2      (POS2)  (Expansion Header Pin 12)
PC5 - OPAMP_N0      (Vin)   (Expansion Header Pin 9)
PD5 - OPAMP_OUT2 #0 (Vout2) (Expansion Header Pin 14)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PC4 - OPAMP_P0      (POS0)  (Expansion Header Pin 7)
PD6 - OPAMP_P1      (POS1)  (Expansion Header Pin 16)
PD4 - OPAMP_P2      (POS2)  (Expansion Header Pin 12)
PC5 - OPAMP_N0      (Vin)   (Expansion Header Pin 9)
PD5 - OPAMP_OUT2 #0 (Vout2) (Expansion Header Pin 14)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PC4 - OPAMP_P0      (POS0)  (Expansion Header Pin 3)
PD6 - OPAMP_P1      (POS1)  (Expansion Header Pin 16)
PD4 - OPAMP_P2      (POS2)  (Expansion Header Pin 12)
PC5 - OPAMP_N0      (Vin)   (Expansion Header Pin 5)
PD5 - OPAMP_OUT2 #0 (Vout2) (Expansion Header Pin 14)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PD10 - OPA0_P   (POS0)  (APORT3XCH2)   (Expansion Header Pin 12)
PD11 - OPA1_P   (POS1)  (APORT4XCH3)   (Expansion Header Pin 14)
PC10 - OPA2_P   (POS2)  (APORT1XCH10)  (Expansion Header Pin 16)
PA4  - OPA0_N   (Vin)   (Negative Pad) (Breakout Pads Pin 13)
PC9  - OPA2_OUT (Vout2) (APORT1YCH9)   (Expansion Header Pin 3)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PD10 - OPA0_P   (POS0)  (APORT3XCH2)   (Expansion Header Pin 7)
PD11 - OPA1_P   (POS1)  (APORT4XCH3)   (Expansion Header Pin 9)
PC10 - OPA2_P   (POS2)  (APORT1XCH10)  (Expansion Header Pin 15)
PA2  - OPA0_P   (Vin)   (Positive Pad) (Breakout Pads Pin 35)
PC9  - OPA2_OUT (Vout2) (APORT1YCH9)   (Expansion Header Pin 13)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PC8 - OPA0_P   (POS0)  (APORT1XCH8)   (Expansion Header Pin 8)
PC9 - OPA1_P   (POS1)  (APORT2XCH9)   (Expansion Header Pin 10)
PA0 - OPA2_P   (POS2)  (APORT3XCH8)   (Expansion Header Pin 12)
PA4 - OPA0_N   (Vin)   (Negative Pad) (Breakout Pads Pin 14)
PA1 - OPA2_OUT (Vout2) (APORT3YCH9)   (Expansion Header Pin 14)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PC8 - OPA0_P   (POS0)  (APORT1XCH8)   (Expansion Header Pin 8)
PC9 - OPA1_P   (POS1)  (APORT2XCH9)   (Expansion Header Pin 10)
PA4 - OPA0_N   (Vin)   (Negative Pad) (Breakout Pads Pin 14)
PA1 - OPA1_OUT (Vout1) (APORT3YCH9)   (Expansion Header Pin 14)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PD10 - OPA0_P   (POS0)  (APORT3XCH2)   (Expansion Header Pin 7)
PD11 - OPA1_P   (POS1)  (APORT4XCH3)   (Expansion Header Pin 9)
PC10 - OPA2_P   (POS2)  (APORT1XCH10)  (Expansion Header Pin 15)
PA2  - OPA0_P   (Vin)   (Positive Pad) (Breakout Pads Pin 35)
PC9  - OPA2_OUT (Vout2) (APORT1YCH9)   (Expansion Header Pin 13)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC8 - OPA0_P   (POS0)  (APORT1XCH8)   (Expansion Header Pin 8)
PC9 - OPA1_P   (POS1)  (APORT2XCH9)   (Expansion Header Pin 10)
PA0 - OPA2_P   (POS2)  (APORT3XCH8)   (Expansion Header Pin 12)
PA4 - OPA0_N   (Vin)   (Negative Pad) (Breakout Pads Pin 14)
PA1 - OPA2_OUT (Vout2) (APORT3YCH9)   (Expansion Header Pin 14)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PC8 - OPA0_P   (POS0)  (APORT1XCH8)   (Expansion Header Pin 8)
PC9 - OPA1_P   (POS1)  (APORT2XCH9)   (Expansion Header Pin 10)
PA4 - OPA0_N   (Vin)   (Negative Pad) (Breakout Pads Pin 14)
PA1 - OPA1_OUT (Vout1) (APORT3YCH9)   (Expansion Header Pin 14)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PD10 - OPA0_P   (POS0)  (APORT3XCH2)   (Expansion Header Pin 7)
PD11 - OPA1_P   (POS1)  (APORT4XCH3)   (Expansion Header Pin 9)
PC10 - OPA2_P   (POS2)  (APORT1XCH10)  (Expansion Header Pin 15)
PA2  - OPA0_P   (Vin)   (Positive Pad) (Breakout Pads Pin 35)
PC9  - OPA2_OUT (Vout2) (APORT1YCH9)   (Expansion Header Pin 13)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC8 - OPA0_P   (POS0)  (APORT1XCH8)   (Expansion Header Pin 8)
PC9 - OPA1_P   (POS1)  (APORT2XCH9)   (Expansion Header Pin 10)
PA0 - OPA2_P   (POS2)  (APORT3XCH8)   (Expansion Header Pin 12)
PA4 - OPA0_N   (Vin)   (Negative Pad) (Breakout Pads Pin 14)
PA1 - OPA2_OUT (Vout2) (APORT3YCH9)   (Expansion Header Pin 14)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PC8 - OPA0_P   (POS0)  (APORT1XCH8)   (Expansion Header Pin 8)
PC9 - OPA1_P   (POS1)  (APORT2XCH9)   (Expansion Header Pin 10)
PA4 - OPA0_N   (Vin)   (Negative Pad) (Breakout Pads Pin 14)
PA1 - OPA1_OUT (Vout1) (APORT3YCH9)   (Expansion Header Pin 14)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE8  - OPA0_P   (POS0)  (APORT3XCH8)   (Expansion Header Pin 12)
PE9  - OPA1_P   (POS1)  (APORT4XCH9)   (Expansion Header Pin 14)
PA12 - OPA2_P   (POS2)  (APORT1XCH12)  (Expansion Header Pin 3)
PC5  - OPA0_N   (Vin)   (Negative Pad) (Expansion Header Pin 9)
PA13 - OPA2_OUT (Vout2) (APORT1YCH13)  (Expansion Header Pin 5)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD6  - OPA1_P   (POS1)  (Positive Pad) (Expansion Header Pin 16)
PA14 - OPA2_P   (POS2)  (APORT1XCH14)  (Breakout Pads Pin 5)
PF0  - OPA3_P   (POS3)  (APORT3XCH16)  (Breakout Pads Pin 5)
PD7  - OPA1_N   (Vin)   (Negative Pad) (Expansion Header Pin 15)
PF1  - OPA3_OUT (Vout3) (APORT3YCH17)  (Breakout Pads Pin 6)
