PCNT_SINGLE_OVERSAMPLING

 This project demonstrates the single input oversampling mode of the pulse counter
 with interrupts. LFA clock is used as the pulse counter clock in this example. The
 program generates an interrupt whenever the pulse counter goes below zero.
 In this example, each press of Push Button PB0 will decrease the counter value by 1.
 The initial value of the counter and the reload value from the top register is set
 by the user.

 When testing, observe how the value of the counter register is counting down whenever
 BTN0 is pressed.

How To Test:
1. Build the project and download to the Starter Kit
2. Press Button 0 five times
3. Observe LED0 toggle
4. Press Button 0 six times for any future tries
5. Observe LED0 toggle

When first run, an interrupt is requested after button 0 is pressed 5 times, after which
it is requested every 6 presses.

Peripherals Used:
LFRCO - 32768 Hz
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

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PB9 - Push Button PB0

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD7 - LED0
PD8 - Push Button PB0

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


For Gecko (Gxxxs_STK):

 This project demonstrates the single input oversampling mode of the pulse counter
 using interrupts. LFACLK is used as the pulse counter clock in this example. The
 program generates an interrupt whenever the pulse counter goes below zero.
 In this example, each press of Push Button PB0 will decrease the counter value by 1.
 The initial value of the counter and the reload value from the top register are set
 by the user.

 When testing, observe how the value of the counter register is counting down whenever
 BTN0 is pressed.

 Note for Gecko Gxxx devices, the PRS cannot be used with the PCNT

Prepare to Test:
1. Wire PC5 on expansion header(pin 5) to PE0 on the breakout pad
OR connect a function generator to PC5. The frequency of the function generator
should be configured so user can observe the change in pulses and the toggling
of LED0.

How To Test:
1. Build the project and download to the Starter Kit
2. Press Button 0 11 times, each press toggle PC5, observe counter in debug mode
3. Observe LED0 toggle
4. Press Button 0 12 times for future tries, corresponding to 6 pulses in PC5
5. Observe LED0 toggle

When the program is first flashed on to the board, the PRS gated one press, therefore user only
need to press Button 0 11 times, after that it will require 12 presses.

Peripherals Used:
LFRCO - 32768 Hz
PCNT


Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - LED0
PB9 - Push Button PB0


