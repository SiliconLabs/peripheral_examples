acmp_polled

This project demonstrates using the analog comparator (ACMP) peripheral to 
compare an analog input to an internal reference. In this example, the analog
input is GPIO port PD02, which corresponds to PB0 and EXP_HEADER7 on the WSTK,
and is being compared to the 1.25V internal reference. The ACMP output is 
routed to GPIO PD03, which corresponds to EXP_HEADER9 on the WSTK. When the voltage
on the analog input is lower than 1.25V, the ACMP output is 0 and EXP_HEADER9 is low.
When the voltage on the analog input is higher than 1.25V, the ACMP output is 1 and
EXP_HEADER9 is at IOVDD (3.3V). The ACMP status register is polled continuously in an
infinite while loop. When the ACMP status register analog comparator output value 
(ACMPOUT bit) goes high, GPIO port PB01 (LED1 on WSTK) is set high (IOVDD). When the
ACMP status register ACMPOUT bit goes low, GPIO port PB01 (LED1 on WSTK) is cleared (GND).  

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Toggle PB0
4. Observe LED1; observe GPIO PD03 using an oscilloscope
5. Connect an external supply to EXP_HEADER7 on the WSTK (with voltage <= IOVDD 3.3V)
6. Vary the external supply voltage above and below 1.25V
7. Observe LED1; observe GPIO PD03 using an oscilloscope

Peripherals Used:
ACMP         - Full input range from 0 to Vdd
               Low accuracy mode, less current usage
               Hysteresis disabled
               Output 0 when ACMP is inactive
               VREFDIV set to maximum to disable divide

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD02 - GPIO input with filter pulled high, Expansion Header Pin 7, WSTK Pin 4, PB0
PD03 - GPIO Push/Pull output, Expansion Header Pin 9, WSTK Pin 6
PB01 - GPIO Push/Pull output, Expansion Header Pin 13, WSTK Pin 10, LED1