em4_wakeup

For Tiny Gecko:

This project demonstrates the ability for a pin to wake the device from EM4.
When first loading th eproject, the device will enter EM4.
Pressing the external reset button will wake the device from EM4 and cause the LEDs on the STK to
toggle indefinitely.
You need to use an socket board to test this device.

How to Test:
1. Build the project and download to the Starter Kit
2. Observe that the current consumption of the device indicates that it is
   in EM4.
3. Press PA0 on the socket board
4. Observe the LEDs blinking, indicating that the last reset cause was
   exist from EM4

Peripherals Used:
HFRCO  - 19 MHz

Board: Silicon Labs EFM32TG Starter Kit (EFM32TG_STK3300)
Device: EFM32TG840F32
PD7 - LED0
