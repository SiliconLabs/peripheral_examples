pcnt_extclk_quadrature

 This project demonstrates the external quadrature mode of the pulse counter
 peripheral. The program will request an interrupt whenever the quadrature
 decoder detects a change in direction.


How To Test:
1. Build the project and download to the Starter Kit
2. Press BTN1, hold it
3. Press BTN0, hold it
4. Release BTN1, then release BTN0
5. Keep doing the steps above until LED0 turns on, indicating
   the inputs are synced and direction register is 1
6. If you keep on repeating the step above, LED0 will stay on
7. Now do the reverse button, press BTN0, hold; then BTN1, hold;
   release BTN0 then BTN1, press BTN0 then BTN1, when you release BTN0
   LED1 will turn on since direction changed to 0
8. If you reverse the pattern again, LED0 will turn off(toggle), note you
   need to repeat the pattern twice before noticing the change since button
   is reverse logic
9. You may also directly connect PCNT0 S0 pins and S1 pins to a function generator
   This requires modification in code where you need to remove all PRS functions
   This may also require a socket board

Peripherals Used:
PCNT

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PF5 - LED1
PC9 - Push Button PB0
PC10 - Push Button PB1

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PE3 - LED1
PB9 - Push Button PB0
PB10 - Push Button PB1

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC10 - LED0
PC11 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A)
Device: EFM32BG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG13P Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG14P Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A)
Device: EFM32MG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG12P Starter Kit (BRD4161A)
Device: EFM32MG12P432F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG13P Starter Kit (BRD4159A)
Device: EFM32MG13P632F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG14P Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A)
Device: EFM32FG1P133F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG13P Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG14P Starter Kit (BRD4257A)
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

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - LED0
PC2 - LED1
PD5 - Push Button PB0
PC9 - Push Button PB1


