em4_wakeup

For Gecko:

This project demonstrates the ability for a pin to wake the device from EM4.
When first loading the project, the device will enter EM4.
Pressing the external reset button will wake the device from EM4 and cause the LEDs on the STK to
toggle indefinitely.

How To Test:
1. Build the project and download to the Starter Kit
2. Observe that the current consumption of the device indicates that it is
    in EM4.
3. Press the external reset button to exit EM4.
4. Observe the LEDs blinking, indicating that the last reset cause was
    exit from EM4.

Peripherals Used:
HFRCO  - 19 MHz

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - LED0
PC1 - LED1


