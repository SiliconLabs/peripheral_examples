em4_wakeup

For Happy Gecko:

This project demonstrates the ability for a pin to wake the device from EM4.
Under a normal, non-EM4 reset, the device will enter EM4.
Pressing PB0 will wake the device from EM4 and cause the LEDs on the STK to
toggle indefinitely.

How To Test:
1. Build the project and download to the Starter Kit
2. Observe that the current consumption of the device indicates that it is
    in EM4.
3. Press PB0 to exit EM4.
4. Observe the LEDs blinking, indicating that the last reset cause was
    exit from EM4.

Peripherals Used:
HFRCO  - 19 MHz

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF4 - LED0
PF5 - LED1
PC9 - Push Button PB0


