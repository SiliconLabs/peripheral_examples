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
USART1 - Synchronous (SPI) mode, CLK = 1 MHz


Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG1P232F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32BG13 Starter Kit (BRD4104A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG1P133F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32FG13 Starter Kit (BRD4256A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32FG14 Starter Kit(BRD4257A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFM32MG1P232F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32MG13 Starter Kit (BRD4159A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32MG14 Starter Kit(BRD4169B) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10