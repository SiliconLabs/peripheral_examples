async_poll

This example loops back the async uart using polling.  

How To Test:
1. Build the project and download to the Starter Kit
2. Connect UART device (such as CP210x) to STK
3. Type in terminal, lines will be echoed after enter is pressed, or 80
   characters are received 

Peripherals Used:
HFRCO  - 19 MHz
USART0 - 115200 baud, 8-N-1 (Series 1 only)
USART1 - 115200 baud, 8-N-1 (Series 0 only)


Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PC0 - USART1 TX
PC1 - USART1 RX

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFM32MG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFR32MG12 Radio Board (BRD4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG1P133F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG12P433F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG12P332F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
