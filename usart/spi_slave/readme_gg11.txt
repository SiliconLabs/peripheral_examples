spi_slave

This example demonstrates USART as a SPI slave.
Data received on the MOSI pin is processed by an interrupt.
Once moved to a buffer the same interrupt moves data from
the TxBuffer into the TXDATA register which will send data 
to the master on the next transfer.

Note: The TXDATA register for the slave must be preloaded
before the first transfer. Due to USART being synchronous,
whenever the master sends data to the slave, the slave
will send whatever is in it's TXDATA register back. If the first
value is not preloaded the first byte back to the master
on the first transfer will always be junk. 

How to connect the master board to slave board:
Connect master CS to slave CS
Connect master SCLK to slave SCLK
Connect master MOSI to slave MOSI
Connect master MISO to slave MISO

How To Test:
1. Build the project and download to the Starter Kit
2. Build spi_master project and download to a Starter Kit
3. Place breakpoint in slave's USART1_RX_IRQHandler() per comments
4. Run spi_slave 
5. Run spi_master 
6. After hitting the breakpoint, observe RxBuffer[] in IDE variables/expressions window. 
   RxBuffer[] should contain 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09.

Peripherals Used:
HFRCO  - 19 MHz
USART0 - Synchronous (SPI) mode, CLK = 1 MHz


Board: Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A_EFM32GG11)
Device: EFM32GG11B820F2048GL192
PE10 - USART0 MOSI - EXP Header Pin 4
PE11 - USART0 MISO - EXP Header Pin 6
PE12 - USART0 CLK  - EXP Header Pin 8
PE13 - USART0 CS   - EXP Header Pin 10
