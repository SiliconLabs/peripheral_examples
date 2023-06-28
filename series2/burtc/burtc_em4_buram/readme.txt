burtc_em4_buram

This project uses the BURTC (Backup Real Time Counter) to wake the device from
EM4 mode and thus trigger a reset. This project also shows how to use the BURAM
retention registers to have data persist between resets. The number of resets 
triggered by the BURTC will printed via the device's USART and the mainboard's 
JLink CDC UART Port for view in a PC terminal program.

How To Test:
1. Build the project and download it to the Starter Kit
2. Close debug session in IDE
3. Open a terminal program and connect to the COM port associated with Starter Kit's
   Jlink CDC UART Port (see Windows Device Manager) using 115200 baud, 8-N-1
4. Press the reset button the mainboard
5. Follow instructions in the terminal program to enter EM4
6. Observe the number of EM4 wakeups should increase after each EM4 wakeup

Peripherals Used:
BURTC   - Interrupt every ~3 seconds
ULFRCO  - 1000 Hz, BURTC clock source
USART0  - 115200 baud, 8-N-1

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  red LED on radio board, yellow LED0 on mainboard
PD02 -  push button PB0

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB00 -  yellow LED0 on mainboard
PD02 -  push button PB0

Board:  Silicon Labs EFR32ZG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 -  yellow LED0 on mainboard
PB01 -  push button PB0

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02 -  yellow LED0 on mainboard
PB01 -  push button PB0

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06 -  yellow LED0 on mainboard
PB00 -  push button PB0

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB01 -  yellow LED1 on mainboard
PB00 -  push button PB0

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 -  yellow LED0 on mainboard
PB01 -  push button PB0