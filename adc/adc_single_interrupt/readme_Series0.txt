ADC0_single_interrupt

This project demonstrates ADC0 peripheral used with a single-ended
input to take nonblocking measurements, and outputs the results to
the VCOM port via UART.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open any terminal program and connect to the device's VCOM port
4. Observe the measured values on the terminal screen and how they
respond to stimulation of the corresponding EXP header pin (see below)

Peripherals Used:
HFRCO  - 14 MHz
UART0 - 115200 baud, 8-N-1
LEUART0 - 115200 baud, 8-N-1
USART1 - 115200 baud, 8-N-1
ADC - 12-bit, 2.5V internal reference

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD4 - LEUART0 TX
PD5 - LEUART0 RX
PF7 - Board Controller VCOM Enable
PD7 - ADC0 Channel 7 (Expansion Header Pin 4)

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PF2 - USART1 TX
PA0 - USART1 RX
PA9 - Board Controller VCOM Enable
PD7 - ADC0 Channel 7 (Expansion Header Pin 15)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE0 - UART0 TX
PE1 - UART0 RX
PF7 - Board Controller VCOM Enable
PD7 - ADC0 Channel 7 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE0 - UART0 TX
PE1 - UART0 RX
PF7 - Board Controller VCOM Enable
PD7 - ADC0 Channel 7 (Expansion Header Pin 14)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE0 - UART0 TX
PE1 - UART0 RX
PF7 - Board Controller VCOM Enable
PD7 - ADC0 Channel 7 (Expansion Header Pin 14)