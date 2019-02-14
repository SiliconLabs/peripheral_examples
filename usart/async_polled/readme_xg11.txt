async_polled

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
USART0 - 115200 baud, 8-N-1


Board: Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC0 - USART0 TX
PC1 - USART0 RX

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC0 - USART0 TX
PC1 - USART0 RX