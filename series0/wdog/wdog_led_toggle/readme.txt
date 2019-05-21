WDOG

This project demonstrates the functionality of the watchdog timer. It uses 
LEDs and a Push-button to indicate the state of the system. If the LEDs are 
flashing, the system has not undergone a reset. If the Push Button 0 is 
pressed and held for some time, it will trigger a WDOG reset. The code looks
for a reset source in the beginning of the execution and if WDOG is found to 
be the reset source, both the LEDs are turned on and the code does not execute 
any further. 

How To Test:
1. Build the project and download to the Starter Kit
2. LEDs 0 and 1 should be blinking
3. Press and hold Push Button 0 for a period of 3 seconds
4. Both the LEDs are now ON and not blinking anymore, which is indicative of
   a WDOG reset

Peripherals Used:
WDOG - 2 seconds period


Board: Silicon Labs EFM32G Starter Kit (STKXXX)
Device: EFM32G890F128
PC0 - LED0
PC1 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board: Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board: Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PF5 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1

Board: Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board: Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board: Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222f32
PC10 - LED0
PC11 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1


Board:  Silicon Labs EFM32BG1 Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG12 Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG1 Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG12 Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFR32FG14 Starter Kit(BRD4257A)
Device: EFR32FG14P233F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PH10 - LED0
PH13 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1

Board:  Silicon Labs EFM32MG1 Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG12 Starter Kit (BRD4161A)
Device: EFR32MG12P432F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFR32MG13 Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1


Board: Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFM32PG1B Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - LED0
PC2 - LED1
PD5 - Push Button PB0
PC9 - Push Button PB1
