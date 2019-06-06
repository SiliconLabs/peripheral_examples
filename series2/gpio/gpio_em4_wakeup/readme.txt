gpio_em4_wakeup

This project uses GPIO to wake the device from EM4 mode and thus trigger a reset.
Under non-EM4 reset, the device will enter EM4. Pressing PB0 will wake the device 
from EM4 and cause the LEDs on the STK/Radio board to toggle indefinitely. 

How To Test:
1. Build the project and download it to the Starter Kit
2. Close debug session in IDE
3. Observe the current consumption of the device which indicates that device is 
   in EM4. One of the options is to use Energy profiler in Simplicity Studio. 
   While capturing current consumption, set the debug mode to OFF by selecting
   the Jlink device in debug adapters and opening device configuration.
5. Press PB0 to exit EM4.
4. Observe the LEDs blinking, indicating that the last reset cause was exit from 
   EM4. Capture current consumption which implies device is in EM0.

Peripherals Used:
FSRCO  - 20 MHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 - red LED on radio board, yellow LED0 on mainboard
PD02 - push button PB0
