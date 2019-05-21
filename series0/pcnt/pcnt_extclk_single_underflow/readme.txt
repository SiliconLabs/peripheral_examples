PCNT_EXTCLK_SINGLE_UNDERFLOW

 This project demonstrates the external clock single input mode of the pulse counter
 with interrupts. BTN0 is used as the pulse counter clock in this example. The
 program requests an interrupt whenever the pulse counter goes below zero.
 In this example, each press of Push Button PB0 will decrease the counter value by 1.
 The initial value of the counter and the reload value from the top register is set
 by the user.

 When testing, observe how the value of the counter register is counting down whenever
 BTN0 is pressed.

How To Test:
1. Build the project and download to the Starter Kit
2. Press Button 0 three times (two times to sync clock, one time to go below zero)
3. Observe LED0 toggle, note counter originally was zero, now it is five
4. Press Button 0 six times for any future tries
5. Observe LED0 toggle

When first run, an interrupt is requested after button 0 is pressed 3 times,
after which it is requested every 6 presses. This happens because with a low
frequency clock, it needs to be synced for two to three clock cycle before the
counter will take the input. Since push button 0 auto generate a press when
initialized through PRS, it will require two extra more presses.


Peripherals Used:
External Clock
PCNT


Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE2 - LED0
PB9 - Push Button PB0

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PC9 - Push Button PB0

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE2 - LED0
PB9 - Push Button PB0

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD7 - LED0
PD8 - Push Button PB0

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PB9 - Push Button PB0

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC10 - LED0
PC8 - Push Button PB0

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A)
Device: EFM32BG1P232F256GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32BG13P Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32BG14P Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A)
Device: EFM32MG1P232F256GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32MG12P Starter Kit (BRD4161A)
Device: EFM32MG12P432F1024GL125
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32MG13P Starter Kit (BRD4159A)
Device: EFM32MG13P632F512GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32MG14P Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A)
Device: EFM32FG1P133F256GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32FG13P Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32FG14P Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PH10 - LED0
PC8 - Push Button PB0

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - LED0
PD5 - Push Button PB0

