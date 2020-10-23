gpio_prs_ldma_usart

This example demonstrates using LDMA with PRS where USART
packet transfers are triggered every push of Push Button 0.

7 bytes, are transmitted on USART TX Pin.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect board to CP2102, or any usb to uart bridge controller (for CP2102, connect the board's USART TX pin to RXD and the USART RX pin to TXD).
3. Read serial data from the controller using a terminal emulator like putty.
4. Every press of Push Button 0 should cause 7 byte of text to be transmitted from the usart.
Observe usart transmissions in the terminal emulator.

Peripherals Used:
USART0 - Asynchronous
PRS
LDMA
GPIO

Board: Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC0 - USART0 TX - EXP Header Pin 16
PC1 - USART0 RX - EXP Header Pin 15
PC8 - Push Button PB0

Board: Silicon Labs EFM32PG1B Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA0 - USART0 TX - EXP Header Pin 12
PA1 - USART0 RX - EXP Header Pin 14
PF6 - Push Button PB0

Board: Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PA0 - USART0 TX - P9
PA1 - USART0 RX - P11
PF6 - Push Button PB0

Board: Silicon Labs EFR32BG1 Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PA0 - USART0 TX - P9
PA1 - USART0 RX - P11
PF6 - Push Button PB0

Board: Silicon Labs EFR32FG1 Starter Kit (BRD4251B)
Device: EFR32FG1P133F256GM48
PA0 - USART0 TX - P9
PA1 - USART0 RX - P11
PF6 - Push Button PB0

Board: Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PF6 - Push Button PB0

Board: Silicon Labs EFR32MG12 Starter Kit (BRD4001A)
Device: EFR32MG12P433F1024GL125
PA0 - USART0 TX - P33
PA1 - USART0 RX - P34
PF6 - Push Button PB0

Board: Silicon Labs EFR32BG12 Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PA0 - USART0 TX - P33
PA1 - USART0 RX - P34
PF6 - Push Button PB0

Board: Silicon Labs EFR32FG12 Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PA0 - USART0 TX - P33
PA1 - USART0 RX - P34
PF6 - Push Button PB0

Board: Silicon Labs EFR32MG13 Starter Kit (BRD4001A)
Device: EFR32MG13P632F512GM48
PA0 - USART0 TX - EXP Header Pin 12
PA1 - USART0 RX - EXP Header Pin 14
PF6 - Push Button PB0

Board: Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PA0 - USART0 TX - P9
PA1 - USART0 RX - P11
PF6 - Push Button PB0

Board: Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PA0 - USART0 TX - P9
PA1 - USART0 RX - P11
PF6 - Push Button PB0

Board: Silicon Labs EFR32MG14 Starter Kit (BRD4169A)
Device: EFR32MG14P733F256GM48
PA0 - USART0 TX - P9
PA1 - USART0 RX - P11
PF6 - Push Button PB0

Board: Silicon Labs EFR32FG14 Starter Kit (BRD4258A)
Device: EFR32FG14P233F256GM48
PA0 - USART0 TX - P9
PA1 - USART0 RX - P11
PF6 - Push Button PB0

Board: Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC0 - USART0 TX - EXP Header Pin 3
PC1 - USART0 RX - EXP Header Pin 5
PD5 - Push Button PB0
