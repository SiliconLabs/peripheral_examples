rtcc_alarm_set

This project demonstrates the RTCC peripheral by setting an alarm.
Information is sent to and from the device via the device's USART and 
the STK's VCOM port. After the current date/time and the alarm date/time
are set, LED1 will turn on when the alarm occurs.

How To Test:
1. Build the project and download to the Starter Kit
2. Open any terminal program and connect to the device's VCOM port
   (note: if using puTTY, Local echo must be set to force on)
3. Follow the instructions on the terminal

Peripherals Used:
LFXO  - 32.76kHz
USART0 - 115200 baud, 8-N-1
RTCC1 

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFR32MG12 Radio Board (SLWRB4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P432F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG12P433F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG1P133F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG12P332F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable