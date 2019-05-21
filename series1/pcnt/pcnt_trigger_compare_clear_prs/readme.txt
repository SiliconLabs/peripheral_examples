PCNT_TRIGGER_COMPARE_CLEAR_PRS

 This project demonstrates the trigger compare and clear functionality
 of the pulse counter using interrupts. The program requests an interrupt 
 whenever the pulse counter clock (PRS in this example) triggers a
 compare and clear command and the result is the counter less than 
 or equal to the top value. Note the top value is set to 5 in this
 example. This feature is only for series 1 boards.


How To Test:
1. Build the project and download to the Starter Kit
2. Run in the debugger or just the board 
3. Press Button 1, notice LED0 toggle, indicating interrupt triggered
4. Press Button 0 six times (five times when you first run the code)
5. Press Button 1, notice that nothing happen since counter > top
5. Press Button 0 for 5 times or less
6. Press Button 1, notice that LED0 toggle, indicating interrupt triggered


Peripherals Used:
LFRCO - 32768 Hz
PCNT

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A)
Device: EFM32BG1P232F256GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG13P Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32BG14P Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A)
Device: EFM32MG1P232F256GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG12P Starter Kit (BRD4161A)
Device: EFM32MG12P432F1024GL125
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG13P Starter Kit (BRD4159A)
Device: EFM32MG13P632F512GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32MG14P Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A)
Device: EFM32FG1P133F256GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG13P Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32FG14P Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PF4 - LED0
PF6 - Push Button PB0
PF7 - Push Button PB1

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PH10 - LED0
PC8 - Push Button PB0
PC9 - Push Button PB1

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - LED0
PD5 - Push Button PB0
PC9 - Push Button PB1


