em4_wakeup

For Giant Gecko 1, Wonder Gecko, and Leopard Gecko:

This project demonstrates the ability for a pin to wake the device from EM4.
Under a normal, non-EM4 reset, the device will enter EM4.
Grounding PA0 will wake the device from EM4 and cause the output pin to toggle indefinitely.
Note you have to use a socket board since PA0 is not provided as an expansion header pin
You also have to map one of the output pin on the socket board to its LED by using wire connections

How To Test:
1. Build the project and download to the Starter Kit
2. Observe that the current consumption of the device indicates that it is
    in EM4.
3. Attach PA0 to ground to exit EM4.
4. Observe the LEDs blinking, indicating that the last reset cause was
    exit from EM4.

Peripherals Used:
HFRCO  - 19 MHz

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE10 - mapped to LED0 on the socket board
PA0 - EM4 Wake-Up Pin

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE10 - mapped to LED0 on the socket board
PE3 - LED1
PA0 - EM4 Wake-Up pin

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE10 - mapped to LED0 on the socket board
PA0 - EM4 Wake-Up pin


