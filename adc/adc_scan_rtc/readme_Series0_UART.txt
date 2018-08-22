adc_scan_rtc

This project demonstrates the ADC0 peripheral used in scanning mode with
multiple single-ended inputs to take measurements automatically without
CPU intervention. One of the inputs corresponds to an EXP header pin and
the other is routed to an unexposed pin for demonstration purposes. The
device is woken up from EM2 sleep every 500ms by the RTC peripheral to take
two ADC measurements and output the results to the VCOM port via UART.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open any terminal program and connect to the device's VCOM port
4. Observe the measured values on the terminal screen and how the value of
CH7 responds to stimulation of the corresponding EXP header pin (see below)

Peripherals Used:
HFRCO - 14 MHz
LFRCO - 32.768 KHz
ADC0  - 13 MHz
UART0 - 115200 baud, 8-N-1
RTC

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PE0 - UART0 TX
PE1 - UART0 RX
PF7 - Board Controller VCOM Enable
PD7 - ADC0 Channel 7 (Expansion Header Pin 14)
PD0 - ADC0 Channel 0

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PE0 - UART0 TX
PE1 - UART0 RX
PF7 - Board Controller VCOM Enable
PD7 - ADC0 Channel 7 (Expansion Header Pin 14)
PD0 - ADC0 Channel 0

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PE0 - UART0 TX
PE1 - UART0 RX
PF7 - Board Controller VCOM Enable
PD7 - ADC0 Channel 7 (Expansion Header Pin 14)
PD0 - ADC0 Channel 0