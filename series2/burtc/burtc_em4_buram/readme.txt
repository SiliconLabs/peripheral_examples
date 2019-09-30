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
BURTC  - Interrupt every ~3 seconds
ULFRCO - 1000 Hz, BURTC clock source
USART0 - 115200 baud, 8-N-1

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 - red LED on radio board, yellow LED0 on mainboard
PD02 - push button PB0

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB00 - yellow LED0 on mainboard
PD02 - push button PB0


