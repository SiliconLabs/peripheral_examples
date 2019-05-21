ADC_Single_Letimer_Interrupt

This project demonstrates ADC0 peripheral used with a single-ended input 
to take nonblocking measurements requested periodically by the LETIMER.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "millivolts" to the Expressions window
4. Observe the measured values in the expressions window and how they
respond to stimulation of the corresponding EXP header pin (see below)

Peripherals Used:
Series 1
HFPER   - 19 MHz
ADC     - 16 MHz, 12-bit resolution, 2.5V internal reference
LETIMER - 1  kHz interrupt frequency, 19 MHz clock frequency

Series 0
HFPER   - 14 MHz
ADC     - 13 MHz, 12-bit resolution, 2.5V internal reference
LETIMER - 1  kHz interrupt frequency, 14 MHz clock frequency


Board:  Silicon Labs EFM32G Starter Kit (G8xx-STK)
Device: EFM32G890F128
PD7 - ADC0 Channel 7 (Expansion Header Pin 15)

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD7 - ADC0 Channel 7 (Expansion Header Pin 17)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD7 - ADC0 Channel 7 (Expansion Header Pin 17)

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD7 - ADC0 Channel 7 (Expansion Header Pin 15)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD7 - ADC0 Channel 7 (Expansion Header Pin 17)

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 3)

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG1P232F256GM48
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG12P332F1024GL125
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 13)

Board:  Silicon Labs EFR32BG13 Radio Board (SLWRB4104A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFR32BG14 Radio Board (SLWRB4105A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG14P732F256GM48
PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG1P133F256GM48
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG12P433F1024GL125
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 13)

Board:  Silicon Labs EFR32FG13 Radio Board (SLWRB4256A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFR32FG14 Radio Board (SLWRB4257A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48
PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P232F256GM48
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFR32MG12 Radio Board (SLWRB4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P432F1024GL125
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 13)

Board:  Silicon Labs EFR32MG13 Radio Board (SLWRB4159A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48
PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 10)

Board:  Silicon Labs EFR32MG14 Radio Board (SLWRB4169A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE11- ADC0 Port 4X Channel 11 (Expansion Header Pin 6)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - ADC0 Port 0X Channel 2 (Expansion Header Pin 7)

