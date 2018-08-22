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

Board: Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC0 - USART0 TX
PC1 - USART0 RX
