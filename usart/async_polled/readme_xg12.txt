async_polled

This example echo's back lines written over the virtual com port to the board
using async usart by polling for Rx Data. Once a line is received or 80 characters
the board will echo back what was written.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect UART device (such as CP210x) to STK
3. Type in terminal, lines will be echoed after enter is pressed, or 80
   characters are received 

Peripherals Used:
HFRCO  - 19 MHz
USART2 - 115200 baud, 8-N-1 


Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG12P332F1024GL125
PA6 - USART2 TX
PA7 - USART2 RX

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG12P433F1024GL125
PA6 - USART2 TX
PA7 - USART2 RX

Board:  Silicon Labs EFR32MG12 Radio Board (BRD4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
PA6 - USART2 TX
PA7 - USART2 RX

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA6 - USART2 TX
PA7 - USART2 RX
