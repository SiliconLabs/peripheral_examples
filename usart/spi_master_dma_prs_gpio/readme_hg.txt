spi_master_dma_prs_gpio

This example demonstrates using DMA with SPI as the master where 
packet transfers are triggered every push of Push Button 0.

10 bytes, 0x00 - 0x09, are transmitted on MOSI.
10 bytes are received on MISO. Expected values are 0xA0 - 0xA9.

How to connect the master board to slave board:
Connect master CS to slave CS
Connect master SCLK to slave SCLK
Connect master MOSI to slave MOSI
Connect master MISO to slave MISO

How To Test:
1. Build the project and download to the Starter Kit
2. Build spi_slave project and download to a Starter Kit
3. Place breakpoint in master's main() per comments
4. Run spi_dma_slave 
5. Run spi_dma_master in debug mode 
6. Every press of Push Button 0 should cause the master to both send and receive one packet. 
Observe RxBuffer[] in IDE variables/expressions window.
After ten presses RxBuffer[] should contain:
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9.

Peripherals Used:
HFRCO  - 19 MHz
USART0 - Synchronous (SPI) mode, CLK = 1 MHz

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PE10 - USART0 MOSI - EXP Header Pin 4
PE11 - USART0 MISO - EXP Header Pin 6
PE12 - USART0 CLK  - EXP Header Pin 8
PE13 - USART0 CS   - EXP Header Pin 10
PC9 - Push Button PB0