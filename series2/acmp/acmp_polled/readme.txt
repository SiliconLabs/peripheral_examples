acmp_polled

This project demonstrates using the analog comparator (ACMP) peripheral to 
compare an analog input to an internal reference. In this example, ACMP pos input
is routed to a GPIO input pin and ACMP neg input is routed to the 1.25V internal reference.
The ACMP output is also routed to a GPIO output pin. When the voltage on ACMP pos input
is lower than 1.25V, the ACMP output pin is low (GND). When the voltage on ACMP pos input is 
higher than 1.25V, the ACMP output pin is high (IOVDD). The ACMP status register is  
polled continuously in an infinite while loop. When the ACMP status register analog 
comparator output value (ACMPOUT bit) goes high, an alternative GPIO output pin is set high 
(IOVDD). When the ACMP status register ACMPOUT bit goes low, the alternative GPIO output pin
is cleared (GND).  

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Toggle push button PB0 on WSTK
4. Observe LED; observe ACMP output GPIO using an oscilloscope
5. Connect an external supply to ACMP pos input GPIO (with voltage <= IOVDD 3.3V)
6. Vary the external supply voltage above and below 1.25V
7. Observe LED; observe ACMP output GPIO using an oscilloscope

    NOTE: Although push button 0 can be used to supply an input signal to the ACMP,
          mechanical button bounce may cause extra rising and/or falling edges 
          on the ACMP input, resulting in confusing or undefined behavior on the 
          output indicator pin.  For best results, please use a clean input signal 
          to the ACMP positive input.  

Peripherals Used:
ACMP   - Full input range from 0 to Vdd
         Low accuracy mode, less current usage
         Hysteresis disabled
         Output 0 when ACMP is inactive
         VREFDIV set to maximum to disable divide

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD02 -  ACMP positive input, push button PB0, Expansion Header Pin 7, WSTK Pin 4
PD03 -  ACMP output, Expansion Header Pin 9, WSTK Pin 6
PB01 -  LED1, Expansion Header Pin 13, WSTK Pin 10


Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  ACMP positive input, push button PB0
PB03 -  ACMP output, push button PB1
PB02 -  LED0, Expansion Header Pin 13, WSTK Pin 10

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  ACMP positive input, push button PB0, WSTK Pin 17
PB02 -  LED0, WSTK Pin 19

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB01 -  ACMP positive input, push button PB1, WSTK Pin 21
PC06 -  LED0, ACMP output, WSTK Pin 27

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 -  LED0, WSTK Pin 4, Expansion Header Pin 7
PB01 -  ACMP positive input, push button PB1, WSTK Pin 6, Expansion Header Pin 9
PB03 -  ACMP output, WSTK Pin 13, Expansion Header Pin 16

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  ACMP positive input, push button PB0, WSTK Pin 17
PB02 -  LED0, WSTK Pin 19
PB03 -  ACMP output, push button PB1, WSTK Pin 21