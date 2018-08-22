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
HFRCO  - 19 MHz
USART0 - 115200 baud, 8-N-1
ADC - 12-bit, 2.5V internal reference

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable
PC9 - ADC0 Port 1Y Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable
PC9 - ADC0 Port 1Y Channel 9 (Expansion Header Pin 3)

Board:  Silicon Labs EFR32MG12 Radio Board (SLWRB4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P432F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable
PC9 - ADC0 Port 1Y Channel 9 (Expansion Header Pin 13)

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable
PC9 - ADC0 Port 1Y Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG12P433F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable
PC9 - ADC0 Port 1Y Channel 9 (Expansion Header Pin 13)

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG1P133F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable
PC9 - ADC0 Port 1Y Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG12P332F1024GL125
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable
PC9 - ADC0 Port 1Y Channel 9 (Expansion Header Pin 13)

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG1P232F256GM48
PA0 - USART0 TX
PA1 - USART0 RX
PA5 - Board Controller VCOM Enable
PC9 - ADC0 Port 1Y Channel 9 (Expansion Header Pin 10)