RMU Limited Reset

This project showcases the Limited Reset capability of the Pearl Gecko, 
Blue Gecko, Mighty Gecko, and Flex Gecko kits. In the Limited Reset mode, 
after a soft reset, everything is reset with the exception of Cryotimer, 
Debugger, RTCC, VMON and parts of CMU, RMU and EMU. This example demonstrates 
the Limited Reset capability using watchdog reset and RTCC. 
USART0 is to connected the board controller, which enumerates as a
VCOM port when connected to a PC. This displays the RTCC counts on any serial
terminal program. It can be seen here that despite the wdog reset, the value of
the RTCC remains unchanged.

How To Test:
1. Build the project and download to the Starter Kit
2. Open any terminal program and connect to the device's VCOM port
3. Watch the RTCC values on the terminal screen
4. Press the reset button on the STK
5. The terminal screen should display the reset source and the RTCC's value 
will not restart at 0.

Peripherals Used:
HFRCO  - 19 MHz
USART0 - 115200 baud, 8-N-1
WDOG - 8 second period

Board: Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32BG1 Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32BG12 Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32MG1 Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32MG12 Starter Kit (BRD4161A)
Device: EFR32MG12P432F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32MG1 Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32MG12 Starter Kit (BRD4161A)
Device: EFR32MG12P432F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable


