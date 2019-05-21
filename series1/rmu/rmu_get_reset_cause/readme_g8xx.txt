RMU Reset Sources

This readme applies for main_G8xx which is used with Gecko kits.

This project demonstrates three different reset sources - POR, Pin Reset 
and System Reset Request. GPIO Push Button (PB0) and LEDs (LED0 & LED1) 
are used to initiate and indicate different resets respectively. A specific 
pattern of LEDs indicates a specific reset source. 

------------------------------------------------------------
RSTSRC                LED behavior
------------------------------------------------------------
Power on Reset        LED0 is ON           
Pin Reset             LED1 is ON
System Reset          LED0 and LED1 toggle at 10Hz freq
-------------------------------------------------------------

How To Test:
1. Build the project and download to the Starter Kit
2. Since the device undergoes a system reset in debug, you will see LED0 and 
   LED1 toggling at 10Hz frequency
3. Click the Reset button on the STK - LED1 will turn ON
4. To initiate another system reset, press PB0 and the LEDs will toggle at
   10Hz frequency again
5. Temporarily disconnect power or power off device. This will
   execute a POR to the MCU - LED0 will turn ON

Peripherals Used:
HFRCO  - 19 MHz

Board: Silicon Labs EFM32G Starter Kit (STKXXX)
Device: EFM32G890F128
PC0 - LED0
PC1 - LED1
PB9 - Push Button PB0
