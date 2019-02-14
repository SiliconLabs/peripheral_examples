spi_master_dma_prs_letimer

This example demonstrates using DMA with SPI as the master where 
packet transfers are triggered using the letimer. The letimer is
initialized to pulse for a set number of times equal to the length
of TxBuffer and after each pulse a prs signal is generated that
triggers a packet to be sent from the master to the slave. Upon
receiving, the slave will send a packet back to the master which
will be handled by a DMA transfer from the RX data register
to the RxBuffer whenever the RXDATAV flag goes high. 

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
5. Run spi_dma_master 
6. Every press of Push Button 0 should cause the master to both send and receive one packet. 
Observe RxBuffer[] in IDE variables/expressions window.
After ten presses RxBuffer[] should contain 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9.

Peripherals Used:
HFRCO    - 19 MHz
USART1   - Synchronous (SPI) mode, CLK = 1 MHz
LDMA     - Channels: 0, 1
LETIMER0 - running in pulse mode
PRS      - Allowing LETIMER0 to trigger DMA transfers every positive edge on a pulse

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG12P332F1024GL125
PA6 - USART1 MOSI - EXP Header Pin 4
PA7 - USART1 MISO - EXP Header Pin 6
PA8 - USART1 CLK  - EXP Header Pin 8
PA9 - USART1 CS   - EXP Header Pin 10

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG12P433F1024GL125
PA6 - USART2 MOSI - EXP Header Pin 4
PA7 - USART2 MISO - EXP Header Pin 6
PA8 - USART2 CLK  - EXP Header Pin 8
PA9 - USART2 CS   - EXP Header Pin 10

Board:  Silicon Labs EFR32MG12 Radio Board (BRD4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
PA6 - USART2 MOSI - EXP Header Pin 4
PA7 - USART2 MISO - EXP Header Pin 6
PA8 - USART2 CLK  - EXP Header Pin 8
PA9 - USART2 CS   - EXP Header Pin 10


Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA6 - USART2 MOSI - EXP Header Pin 4
PA7 - USART2 MISO - EXP Header Pin 6
PA8 - USART2 CLK  - EXP Header Pin 8
PA9 - USART2 CS   - EXP Header Pin 10

