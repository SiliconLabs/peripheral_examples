acmp_pin_output

This project demonstrates using the analog comparator (ACMP) peripheral to 
compare an analog input to an internal reference. In this example, ACMP pos input
is routed to a GPIO input pin and ACMP neg input is routed to the 1.25V internal reference.
The ACMP output is also routed to a GPIO output pin. When the voltage on ACMP pos input
is lower than 1.25V, the ACMP output pin is low (GND). When the voltage on ACMP pos input is 
higher than 1.25V, the ACMP output pin is high (IOVDD). The MCU core is in sleep mode 
in EM3, but the analog comparator remains functional.

How To Test (EFR32xG21):
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  In Project Explorer, right click on the main project directory
4.  In the drop-down menu, select "Profile As Simplicity Energy Profiler Target"
5.  The project will compile, load and start-up, proceeding to the main loop which sits in EM3
6.  Observe the current consumption in Energy Profiler of the kit in EM3 - typically ~15uA
7.  Press and hold push button PB1
8.  Observe the marginally higher current consumption with PB1 is pressed - typically a few
    uA higher than observed in step 6 (current draw through push button pull-up resistor)
9.  Release PB1 and observe the drop in current back to that seen in step 6.
10. Toggle PB1
11. Observe the ACMP output GPIO using an oscilloscope
12. Connect an external supply to ACMP pos input GPIO (with voltage <= IOVDD 3.3V)
13. Repeat steps 7 thru 12, but instead of using PB1, vary the external supply voltage above
    and below 1.25V
    
How To Test (EFR32xG23, EFR32xG24, EFR32xG25, EFR32xG27 and EFR32xG28):   
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  Flash the firmware hex or binary to the target device
4.  Apply a voltage source or waveform to the ACMP input pin and observe LED/GPIO
    status indicator using an osciloscope
    (LED ON/pin high when Vin > Vref, LED OFF/pin low when Vin < Vref)
    
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
PA06 -  ACMP output, Expansion Header Pin 14, WSTK Pin 11
PD03 -  ACMP positive input, push button PB1, Expansion Header Pin 9, WSTK Pin 6

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  ACMP positive input, push button PB0
PB02 -  LED1, Expansion Header Pin 13, WSTK Pin 10

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  ACMP positive input, push button PB0, WSTK Pin 17
PB02 -  LED0, WSTK Pin 19
PB03 -  GPIO output from ACMP, WSTK Pin 21 (optional)

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) +
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB01 -  ACMP positive input, push button PB1, WSTK Pin 21
PC06 -  LED0, GPIO output from ACMP

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 -  LED0, GPIO output from ACMP, WSTK Pin 4, Expansion Header Pin 7
PB01 -  ACMP positive input, push button PB1, WSTK Pin 4, Expansion Header Pin 9

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  ACMP positive input, push button PB0, WSTK Pin 17
PB02 -  LED0, WSTK Pin 19
PB03 -  GPIO output from ACMP, push button PB1, WSTK Pin 21