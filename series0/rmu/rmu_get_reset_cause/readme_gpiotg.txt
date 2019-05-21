RMU Reset Sources

This readme applies for main_GPIO which is used with Tiny Gecko kits.

This project demonstrates four different reset sources - POR, EM4, Pin Reset 
and System Reset Request. GPIO Push Buttons (PB0 & PB1) and LEDs (LED0) 
are used to initiate and indicate different resets respectively. A specific 
pattern of LEDs indicates a specific reset source.

------------------------------------------------------------
RSTSRC                LED behavior
------------------------------------------------------------
Power on Reset        LED0 is ON           
EM4                   LED0 and PC5 are ON
Pin Reset             PC5 is ON
System Reset          LED0 and PC5 toggle at 10Hz freq
-------------------------------------------------------------


For Tiny Gecko:

How To Test:
1. Build the project and download to the Starter Kit
2. Since the device undergoes a system reset in debug, you will see LED0 and 
   PC5 toggling at 10Hz frequency
3. Click the Reset button on the STK - PC5 will set to high
4. To initiate another system reset, press PB1 and the LED and PC5 will toggle at
   10Hz frequency again
5. Press PB0 
    - This puts the device into EM4
    - The device will wake up when PA0 is pressed, LED0 and PC5 will be on, press PB1
      again if you want to enter system reset mode. This step requires an socket board
      as PA0 is not on the breakout pad or expansion header
6. Move the switch on the STK from AEM to BAT and back to AEM. This will
   replicate a POR to the MCU - LED0 will turn ON

Known problem : EM4 wake up bit must be PA0, other bit will cause the device to wake up
                automatically

Peripherals Used:
HFRCO  - 19 MHz


Board: Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PC10 - LED0
PD8 - Push Button PB0
PB11 - Push Button PB1
PA0 - EM4 Wakeup Pin
