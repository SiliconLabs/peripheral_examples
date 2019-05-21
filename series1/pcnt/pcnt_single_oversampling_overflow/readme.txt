PCNT_SINGLE_OVERSAMPLING

 This project demonstrates the single input oversampling mode using
 the LFA clock with interrupts. The program generates an interrupt 
 whenever the pulse counter detects the number of pulses that went
 above a certain threshold. The threshold is defined by the user.
 In this example, Push Button PB0 is used as the pulse counter
 input source.

How To Test:
1. Build the project and download to the Starter Kit
2. Press Button 0 five times
3. Observe LED0 toggle
4. Press Button 0 six times for any future tries
5. Observe LED0 toggle

When first run, an interrupt is requested after button 0 is pressed
5 times, after which it is requested every 6 presses.

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

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PH10 - LED0
PC8 - Push Button PB0

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - LED0
PD5 - Push Button PB0


For Gecko (Gxxx_STK):

 This project demonstrates the single input oversampling mode using
 the LFA clock with interrupts. The program generates an interrupt 
 whenever the pulse counter detects the number of pulses that went
 above a certain threshold. The threshold is defined by the user.
 In this example, Push Button PB0 is used as the pulse counter
 input source.

 Note for Gecko Gxxx devices, the PRS cannot be used with the PCNT.

Prepare to Test:
1. Wire PC5(expansion header 5) to PE0 on the breakout pad
OR wire a function generator to PE0 breakout pad. The function
generator frequency should be low enough for user to track the number of
pulses generated.

How To Test:
1. Build the project and download to the Starter Kit
2. Press BTN0 11 times, each press toggle PC5, PC5 original out is 1
3. Observe LED0 toggle
4. Press BTN0 12 times for each additional tries (equals 6 pulses on PC5)
5. Observe LED0 toggle

When the program is first flashed on the board, there is a default press
gated into the prs, therefore we only need 11 press instead of 12 times.

Peripherals Used:
LFRCO - 32768 Hz
PCNT

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - LED0
PE0 - PCNT S0 Input
PB9 - BTN0
PC5 - Expansion header pin 5


