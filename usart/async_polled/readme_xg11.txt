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
USART0 - 115200 baud, 8-N-1


Board: Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC0 - USART0 TX
PC1 - USART0 RX

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC0 - USART0 TX
PC1 - USART0 RX