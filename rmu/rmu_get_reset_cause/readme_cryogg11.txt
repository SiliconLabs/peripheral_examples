RMU Reset Sources

This readme applies for main_CRYO which is used with Giant Gecko 11 kits.

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

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PH10 - LED0
PH13 - LED1
PC8 - Push Button PB0
PC9 - Push Button PB1


