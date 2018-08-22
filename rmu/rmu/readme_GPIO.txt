RMU Reset Sources

This readme applies for main_GPIO which is used with Zero Gecko and Happy Gecko kits.

This project demonstrates four different reset sources - POR, EM4, Pin Reset 
and System Reset Request. GPIO Push Buttons (PB0 & PB1) and LEDs (LED0 & LED1) 
are used in initiate and indicate different resets respectively. A specific 
pattern of LEDs indicates a specific reset source. 

------------------------------------------------------------
RSTSRC                LED behavior
------------------------------------------------------------
Power on Reset        LED0 is ON           
EM4                   LED0 and LED1 are ON
Pin Reset             LED1 is ON
System Reset          LED0 and LED1 toggle at 10kHz freq
-------------------------------------------------------------

How To Test:
1. Build the project and download to the Starter Kit
2. Since the device undergoes a system reset in debug, you will see LED0 and 
   LED1 toggling at 10kHz frequency
3. Click the Reset button on the STK - LED1 will turn ON
4. To initiate another system reset, press PB1 and the LEDs will toggle at
   10 kHz frequency again
5. Press PB0 
    - This puts the device into EM4 and the cryotimer wakes up the
      device from sleep after 1 second - Both LEDs will turn ON
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

Board: Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PF5 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1