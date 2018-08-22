adc_scan_rtc

This project demonstrates the ADC0 peripheral used in scanning mode with
multiple single-ended inputs to take measurements automatically without
CPU intervention. One of the inputs corresponds to an EXP header pin and
the other is routed to AVDD for demonstration purposes. The ADC measurement
is triggered every 500ms via PRS by the RTCC peripheral. Once the scan
finishes, the ADC interrupt wakes the device up from EM2 in order to
output the results to the VCOM port via USART.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open any terminal program and connect to the device's VCOM port
4. Observe the measured values on the terminal screen and how the input value
responds to stimulation of the corresponding EXP header pin (see below)

Peripherals Used:
HFRCO  - 19 MHz
LFRCO  - 32.768 KHz
ADC0   - 16 MHz
USART0 - 115200 baud, 8-N-1
RTCC

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