adc_scan_prs_gpio_dma

This project demonstrates ADC0 peripheral in EM2 to take single-ended
measurements on multiple pins.  These measurements are requested by gpio
signals and routed through the prs.  Completed conversions are handled by
the DMA, and the results are stored in global variables. Single-ended
inputs are selected from both APORTnX and APORTnY buses. For single-ended 
inputs selected from the APORTnY bus, the ADC performs a negative single-ended
conversion and automatically inverts the result.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "adcBuffer" to the Expressions window
4. Observe the measured values in the expressions window and how they
respond to stimulation of the corresponding EXP header pin (see below)

Peripherals Used:
AUXHFRCO  - 4 MHz
HFRCO - 19 MHz for Series 1, 14 MHz for Series 0
ADC   - 16 MHz for Series 1, 13 MHz for Series 0, 12-bit, 2.5V internal reference
DMA  - Channel 0, ADC0->SCANDATA to adcBuffer
PRS   - Channel 0, gpio to ADC start scan conversion


Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 3)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 16)

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG1P232F256GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG12P332F1024GL125
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG13 Radio Board (SLWRB4104A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG1P133F256GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG12P433F1024GL125
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG13 Radio Board (SLWRB4256A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG14 Radio Board (SLWRB4257A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PA2 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 3)
Analog Input - PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P232F256GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG12 Radio Board (SLWRB4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG13 Radio Board (SLWRB4159A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
Analog Input - PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG14 Radio Board (SLWRB4169A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
Request Input - PF6 - PRS Channel 0 (Push Button 0)
Analog Input - PA2 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 3)
Analog Input - PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
Request Input - PC8 - PRS Channel 0 (Push Button 0)
Analog Input - PE10 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 4)
Analog Input - PE11 - ADC0 Port 4X Channel 11 (Expansion Header Pin 6)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
Request Input - PD5 - PRS Channel 0 (Push Button 0)
Analog Input - PD2 - ADC0 Port 0X Channel 2 (Expansion Header Pin 7)
Analog Input - PD6 - ADC0 Port 0X Channel 6 (Expansion Header Pin 16)
