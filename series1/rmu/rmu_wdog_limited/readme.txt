RMU Limited Reset

This project showcases the Limited Reset capability of series 1 starter kits.
In the Limited Reset mode, after a soft reset, everything is reset with the
exception of Cryotimer, Debugger, RTCC, VMON and parts of CMU, RMU and EMU.
This example demonstrates the Limited Reset capability using watchdog reset and RTCC. 
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
4. Press the reset button on the STK, which will cause the RTCC to restart at 0
5. The terminal screen should display the watchdog reset source every 8 seconds
and the RTCC's value will not restart at 0.

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

Board:  Silicon Labs EFM32BG13P Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32BG14P Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32FG1 Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32FG12 Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32FG13P Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32FG14P Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
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

Board:  Silicon Labs EFM32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32MG14P Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PH4 - USART0 TX
PH5 - USART0 RX
PE1 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD0 - USART0 TX
PD1 - USART0 RX
PA8 - Board Controller VCOM Enable



