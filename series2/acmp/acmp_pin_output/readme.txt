acmp_pin_output

This project demonstrates using the analog comparator (ACMP) peripheral to 
compare an analog input to an internal reference. In this example, the analog
input is GPIO port PD03, which corresponds to push button 1 (PB01) and EXP_HEADER9
on the WSTK, and is being compared to the 1.25V internal reference. In this example,
the ACMP output is routed to a GPIO PA06 (EXP_HEADER14 on WSTK). When the voltage
on the analog input is lower than 1.25V, the ACMP output is 0 and the voltage on PA06
is low (GND). When the voltage on the analog input is higher than 1.25V, the ACMP
output is 1 and the voltage on PA06 is high (IOVDD). The MCU core is in sleep mode 
in EM3, but the analog comparator remains running.

How To Test:
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  In Project Explorer, right click on the main project directory
4.  In the drop-down menu, select "Profile As Simplicity Energy Profiler Target"
5.  The project will compile, load and start-up, proceeding to the main loop which sits in EM3
6.  Observe the current consumption in Energy Profiler of the kit in EM3 - typically ~15uA
7.  Press and hold PB01
8.  Observe the marginally higher current consumption with PB01 is pressed - typically a few
    uA higher than observed in step 6 (current draw through push button pull-up resistor)
9.  Release PB01 and observe the drop in current back to that seen in step 6.
10. Toggle PB01
11. Observe GPIO PA06 using an oscilloscope
12. Connect an external supply to EXP_HEADER9 on the WSTK (with voltage <= IOVDD 3.3V)
13. Repeat steps 7 thru 12, but instead of using PB01, vary the external supply voltage above
    and below 1.25V

Peripherals Used:
ACMP         - Full input range from 0 to Vdd
               Low accuracy mode, less current usage
               Hysteresis disabled
               Output 0 when ACMP is inactive
               VREFDIV set to maximum to disable divide

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PA06 - GPIO Push/Pull output, Expansion Header Pin 14, WSTK Pin 11
PD03 - GPIO input with filter, Expansion Header Pin 9, WSTK Pin 6, PB1
