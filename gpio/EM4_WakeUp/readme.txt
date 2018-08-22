em4_wakeup

For Pearl Gecko 1, Pearl Gecko 12, Zero Gecko, and all EFR32 kits:

This project demonstrates the ability for a pin to wake the device from EM4.
Under a normal, non-EM4 reset, the device will enter EM4.
Pressing PB1 will wake the device from EM4 and cause the LEDs on the STK to
toggle indefinitely.

How To Test:
1. Build the project and download to the Starter Kit
2. Observe that the current consumption of the device indicates that it is
    in EM4.
3. Press PB1 to exit EM4.
4. Observe the LEDs blinking, indicating that the last reset cause was
    exit from EM4.

Peripherals Used:
HFRCO  - 19 MHz

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF4 - LED0
PF5 - LED1
PF7 - Push Button PB1

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF4 - LED0
PF5 - LED1
PF7 - Push Button PB1

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF7 - Push Button PB1

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PF4 - LED0
PF5 - LED1
PF7 - Push Button PB1

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PF4 - LED0
PF5 - LED1
PF7 - Push Button PB1

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PF4 - LED0
PF5 - LED1
PF7 - Push Button PB1

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF7 - Push Button PB1

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PF4 - LED0
PF5 - LED1
PF7 - Push Button PB1

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC10 - LED0
PC11 - LED1
PC9 - Push Button PB1

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

For Giant Gecko 1, Wonder Gecko, and Leopard Gecko:

This project demonstrates the ability for a pin to wake the device from EM4.
Under a normal, non-EM4 reset, the device will enter EM4.
Grounding PA0 will wake the device from EM4 and cause the LEDs on the STK to
toggle indefinitely.

How To Test:
1. Build the project and download to the Starter Kit
2. Observe that the current consumption of the device indicates that it is
    in EM4.
3. Attach PA0 to ground to exit EM4.
4. Observe the LEDs blinking, indicating that the last reset cause was
    exit from EM4.

Peripherals Used:
HFRCO  - 19 MHz

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE2 - LED0
PE3 - LED1
PA0 - EM4 Wake-Up pin

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE2 - LED0
PE3 - LED1
PA0 - EM4 Wake-Up Pin

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE2 - LED0
PE3 - LED1
PA0 - EM4 Wake-Up pin

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