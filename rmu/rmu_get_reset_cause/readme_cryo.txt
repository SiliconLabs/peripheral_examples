RMU Reset Sources

This readme applies for main_CRYO which is used with Pearl Gecko, Blue Gecko, Flex Gecko, and Mighty Gecko kits.

This project demonstrates four different reset sources - POR, EM4, Pin Reset 
and System Reset Request. GPIO Push Buttons (PB0 & PB1) and LEDs (LED0 & LED1) 
are used to initiate and indicate different resets respectively. A specific 
pattern of LEDs indicates a specific reset source. 

------------------------------------------------------------
RSTSRC                LED behavior
------------------------------------------------------------
Power on Reset        LED0 is ON           
EM4                   LED0 and LED1 are ON
Pin Reset             LED1 is ON
System Reset          LED0 and LED1 toggle at 10Hz freq
-------------------------------------------------------------

How To Test:
1. Build the project and download to the Starter Kit
2. Since the device undergoes a system reset in debug, you will see LED0 and 
   LED1 toggling at 10Hz frequency
3. Click the Reset button on the STK - LED1 will turn ON
4. To initiate another system reset, press PB1 and the LEDs will toggle at
   10Hz frequency again
5. Press PB0 
    - This puts the device into EM4 and the cryotimer wakes up the
      device from sleep after 1 second - Both LEDs will turn ON
6. Move the switch on the STK from AEM to BAT and back to AEM. This will
   replicate a POR to the MCU - LED0 will turn ON

Peripherals Used:
HFRCO  - 19 MHz
CRYOTIMER - 1 KHz

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

Board: Silicon Labs EFM32BG13P Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFM32BG14P Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

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

Board:  Silicon Labs EFM32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
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

Board: Silicon Labs EFM32FG13P Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFM32FG14P Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button PB0
PF7 - Push Button PB1

Board: Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
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


