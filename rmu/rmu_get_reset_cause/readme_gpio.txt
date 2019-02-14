RMU Reset Sources

This readme applies for main_GPIO which is used with Zero Gecko and Happy Gecko kits.

This project demonstrates four different reset sources - POR, EM4, Pin Reset 
and System Reset Request. GPIO Push Buttons (PB0 & PB1) and LEDs (LED0 & LED1) 
are used to initiate and indicate different resets respectively. A specific 
pattern of LEDs indicates a specific reset source. Note for Zero Gecko
PB0 and PB1 functionality is switched.

------------------------------------------------------------
RSTSRC                LED behavior
------------------------------------------------------------
Power on Reset        LED0 is ON           
EM4                   LED0 and LED1 are ON
Pin Reset             LED1 is ON
System Reset          LED0 and LED1 toggle at 10Hz freq
-------------------------------------------------------------


For Happy Gecko:

How To Test:
1. Build the project and download to the Starter Kit
2. Since the device undergoes a system reset in debug, you will see LED0 and 
   LED1 toggling at 10Hz frequency
3. Click the Reset button on the STK - LED1 will turn ON
4. To initiate another system reset, press PB0 and the LEDs will toggle at
   10Hz frequency again
5. Press PB1 
    - This puts the device into EM4
    - The device will wake up when PB0 is pressed, both LED will be on, press PB0
      again if you want to enter system reset mode.
6. Move the switch on the STK from AEM to BAT and back to AEM. This will
   replicate a POR to the MCU - LED0 will turn ON

Peripherals Used:
HFRCO  - 19 MHz

Board: Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PF5 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1



For Zero Gecko:

How To Test:
1. Build the project and download to the Starter Kit
2. Since the device undergoes a system reset in debug, you will see LED0 and 
   LED1 toggling at 10Hz frequency
3. Click the Reset button on the STK - LED1 will turn ON
4. To initiate another system reset, press PB1 and the LEDs will toggle at
   10Hz frequency again
5. Press PB0 
    - This puts the device into EM4
    - The device will wake up when PB1 is pressed, both LED will be on, press PB1
      again if you want to enter system reset mode.
6. Move the switch on the STK from AEM to BAT and back to AEM. This will
   replicate a POR to the MCU - LED0 will turn ON

Peripherals Used:
HFRCO  - 19 MHz


Board: Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222f32
PC10 - LED0
PC11 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1


