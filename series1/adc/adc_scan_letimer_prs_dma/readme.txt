adc_scan_letimer_prs_dma

This project demonstrates ADC0 peripheral in EM2 to take multiple single-
ended measurements.  These measurements are requested periodically by the
LETIMER and routed through the prs. Completed conversions are handled by
the LDMA, and the results are stored to global variables. Single-ended
inputs are selected from both APORTnX and Y buses. For single-ended inputs 
selected from the APORTnY bus, the ADC performs a negative single-ended
conversion and automatically inverts the result.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "adcBuffer" to the Expressions window
4. Observe the measured values in the expressions window and how they
respond to stimulation of the corresponding EXP header pin (see below)

Peripherals Used:
AUXHFRCO - 4 MHz
HFPER   - 19 MHz for Series 1, 14 MHZ for Series 0
ADC     - 16 MHz for Series 1, 13 MHZ for Series 0, 12-bit resolution, 
          2.5V internal reference
LETIMER - 1  kHz interrupt frequency
LDMA    - Channel 0, ADC0->SINGLEDATA to adcBuffer
PRS     - Channel 0, gpio to ADC start single conversion


Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 3)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 16)

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG1P232F256GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG12P332F1024GL125
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG13 Radio Board (SLWRB4104A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG1P133F256GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG12P433F1024GL125
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG13 Radio Board (SLWRB4256A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG14 Radio Board (SLWRB4257A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48
PA2 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 3)
PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P232F256GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG12 Radio Board (SLWRB4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG13 Radio Board (SLWRB4159A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG14 Radio Board (SLWRB4169A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
PA2 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 3)
PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE10 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 4)
PE11 - ADC0 Port 4X Channel 11 (Expansion Header Pin 6)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - ADC0 Port 0X Channel 2 (Expansion Header Pin 7)
PD6 - ADC0 Port 0X Channel 6 (Expansion Header Pin 16)
