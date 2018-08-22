async_interrupt

This example echoes back lines sent to it over the virtual
com port by the user using a baudrate of 115200. Once a
new line or 80 characters are received, the RxBuffer is
echoed back. Interrupts are used to read each character as
they come in over the virtual com port.  

How To Test:
1. Build the project and download to the Starter Kit
2. Connect UART device (such as CP210x) to STK
3. Type in terminal, lines will be echoed after enter is pressed, or 80
   characters are received 

Peripherals Used:
HFRCO  - 19 MHz
USART0 - 115200 baud, 8-N-1

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board: Silicon Labs EFR32BG13 Starter Kit (BRD4104A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board: Silicon Labs EFR32BG14 Starter Kit (BRD4105A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG14P732F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG1P133F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board: Silicon Labs EFR32FG13 Starter Kit (BRD4256A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board: Silicon Labs EFR32FG14 Starter Kit(BRD4257A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFM32MG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board: Silicon Labs EFR32MG13 Starter Kit (BRD4159A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board: Silicon Labs EFR32MG14 Starter Kit (BRD4169B) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA0 - USART0 TX
PA1 - USART0 RX





