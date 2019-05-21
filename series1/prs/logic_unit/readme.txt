PRS_Logic

This project demonstrates the built-in PRS logic functions between channels.
PB0 and PB1 are routed to output pins (usually LEDs, but not always.  We'll 
refer to these pins as OUT0 and OUT1).  There are two configurations for this
example: You can use the AND_NEXT option to make Channel 0 AND with Channel 1,
or you can use the OR_PREV option to make Channel 1 OR with Channel 0.

1) You can make Channel 0 (with PB0 and OUT0) AND with Channel 1.  With this 
configuration, the outputs use the following truth table:
PB1   PB0   |   OUT1    OUT0
-----------------------------
0     0     |   0       0
0     1     |   0       0
1     0     |   1       0
1     1     |   1       1

2) Alternativelly, you can make Channel 1 OR with Channel 0.  With this
configuration, the outputs use the following truth table:
PB1   PB0   |   OUT1    OUT0
-----------------------------
0     0     |   0       0
0     1     |   1       1
1     0     |   1       0
1     1     |   1       1


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Press the Push Buttons and notice how the output pins respond

Note: You can switch between AND and OR using the "#define USE_AND" line in the main file.

Peripherals Used:
PRS - Channel 0, PB0 to OUT0
      Channel 1, PB1 to OUT1


Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A)
Device: EFM32BG1P232F256GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFR32BG13 Radio Board (SLWRB4104A)
Device: EFR32BG13P632F512GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFR32BG14 Radio Board (SLWRB4105A) 
Device: EFR32BG14P732F256GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A)
Device: EFM32FG1P133F256GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFR32FG13 Radio Board (SLWRB4256A)
Device: EFR32FG13P233F512GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFR32FG14 Radio Board (SLWRB4257A)
Device: EFR32FG14P233F256GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A)
Device: EFM32MG1P232F256GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFR32MG13 Radio Board (SLWRB4159A)
Device: EFR32MG13P632F512GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFR32MG14 Radio Board (SLWRB4169A)
Device: EFR32MG14P733F256GM48
OUT0 - PRS Channel 0 Route 4  - PF4 (LED0)
OUT1 - PRS Channel 1 Route 4  - PF5 (LED1)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32GG11B820F2048GL192
OUT0 - PRS Channel 0 Route 2  - PC14 (Expansion Header Pin 12)
OUT1 - PRS Channel 1 Route 2  - PC15 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32TG11B520F128GM80
OUT0 - PRS Channel 1 Route 3  - PE12 (Expansion Header Pin 8)
OUT1 - PRS Channel 2 Route 3  - PE13 (Expansion Header Pin 10)
