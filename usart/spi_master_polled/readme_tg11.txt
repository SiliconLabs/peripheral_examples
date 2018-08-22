spi_master_polled

This example demonstrates USART as a SPI master.
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
4. Run spi_slave 
5. Run spi_master 
6. After hitting the breakpoint, observe RxBuffer[] in IDE variables/expressions window.
   RxBuffer[] should contain 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9.

Peripherals Used:
HFRCO  - 19 MHz
USART0 - Synchronous (SPI) mode, CLK = 1 MHz


Board: Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC11 - USART0 MOSI - EXP Header Pin 4
PC10 - USART0 MISO - EXP Header Pin 6
PA12 - USART0 CLK  - EXP Header Pin 8
PC8 -  USART0 CS   - EXP Header Pin 10
