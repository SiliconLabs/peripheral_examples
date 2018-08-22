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
USART1 - Synchronous (SPI) mode, CLK = 1 MHz

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD7 -  USART1 MOSI - EXP Header Pin 4
PD6 -  USART1 MISO - EXP Header Pin 6
PC15 - USART1 CLK  - EXP Header Pin 8
PC14 - USART1 CS   - EXP Header Pin 10