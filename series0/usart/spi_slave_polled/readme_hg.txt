spi_slave_polled

This example demonstrates USART as a SPI slave.
Data received on the MOSI pin is processed by polling.
The TXDATA register is loaded right after receiving data 
to send data to the master each SPI frame on the MISO pin.

How to connect the master board to slave board:
Connect master CS to slave CS
Connect master SCLK to slave SCLK
Connect master MOSI to slave MOSI
Connect master MISO to slave MISO

How To Test:
1. Build the project and download to the Starter Kit
2. Build spi_master project and download to a Starter Kit
3. Place breakpoint in the main while loop when refreshing the RX buffer index as per comments
4. Run spi_slave 
5. Run spi_master 
6. After hitting the breakpoint, observe RxBuffer[] in IDE variables/expressions window. 
   RxBuffer[] should contain 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09.

Peripherals Used:
HFRCO  - 19 MHz
USART0 - Synchronous (SPI) mode, CLK = 1 MHz

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PE10 - USART0 MOSI - EXP Header Pin 3
PE11 - USART0 MISO - EXP Header Pin 5
PE12 - USART0 CLK  - EXP Header Pin 8
PE13 - USART0 CS   - EXP Header Pin 10