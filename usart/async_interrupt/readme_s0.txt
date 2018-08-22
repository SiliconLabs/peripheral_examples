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
USART1 - 115200 baud, 8-N-1

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC0 - USART1 TX
PC1 - USART1 RX