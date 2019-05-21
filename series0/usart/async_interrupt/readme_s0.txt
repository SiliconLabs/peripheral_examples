async_interrupt

This example echoes back lines sent to it over the virtual
com port by the user using a baudrate of 115200. Once a
new line or 80 characters are received, the RxBuffer is
echoed back. 

How To Test:
1. Build the project and download to the Starter Kit
2. Use a USB to serial device (such as the CP210x). Connect the CP210x's RX pin 
   to the board's TX pin. Connect the CP210x's TX pin to the board's RX pin.
   These port and pin mappings are listed below.
3. Open up a serial terminal device such as Termite.
4. In Termite, open the port connected to the CP210x device (check which port
   using Device Manager).
5. After typing in Termite and pressing enter, the input will be echoed back
   (Note: input is limited to RX_BUFFER_SIZE, which in this example is 80
   characters by default).

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