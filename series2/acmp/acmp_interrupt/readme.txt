acmp_interrupt

This project demonstrates interrupt-driven use of the ACMP by comparing
the voltage on an input pin when a button is pressed to the internal
1.25 V reference (VREF).

When the button is pushed, the input voltage is pulled to GND, and the
comparator output goes low. Similarly, when the button is released, the
output is pulled to the common AVDD/IOVDD supply, and the comparator
output goes high.

During operation, the CPU core remains in the deep sleep EM3 mode while
the comparator continues to operate. When the input rises above or falls
below the comparison voltage, the comparator requests an interrupt that
wakes the CPU, which turns LED0 on or off in response before returning
to EM3.

How To Test (EFR32xG21):
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  In Project Explorer, right click on the main project directory
4.  In the drop-down menu, select "Profile As Simplicity Energy Profiler Target"
5.  The project will compile, load and start-up, proceeding to the main loop which waits in EM3
6.  Observe the current consumption in Energy Profiler of the kit in EM3 - typically ~15uA
7.  Press and hold push button PB1
8.  Observe the spike in current as the MCU transitions to EM0 and then back to EM3
9.  Observe the marginally higher current consumption with PB1 is pressed - typically a few
    uA higher than observed in step 6 (current draw through push button pull-up resistor)
10. Release PB1 and observe again the spike in current as the MCU wakes, handles the interrupt
    and returns to EM3.
11. Toggle PB1
12. Observe the interrupt driven GPIO using an oscilloscope
13. Connect an external supply to ACMP pos input GPIO (with voltage <= IOVDD 3.3V)
14. Repeat steps 7 thru 12, but instead of using PB1, vary the external supply voltage above
    and below 1.25V
    
How To Test (all other devices):
1.  Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary).
2.  Build the example project and download it to the target system.
3.  Press and hold button PB0; LED0 will turn on.
4.  When button PB0 is released; LED0 will turn off.

NOTE: Button PB0 provides an easy way to say the example in action, but
mechanical bouncing may cause extra rising or falling edges on the ACMP
input that cross the VREF threshold. Any of these can result in an
interrupt request that changes the state of the GPIO pin driving LED0.

Consider using an active device, such as a programmable power supply or
the output from another processor, to drive the ACMP input so that its
behavior can be observed in the absence of undamped mechanical noise.

Peripherals Used:
ACMP - Full input range from GND to VMCU (common AVDD/IOVDD supply)
       Low accuracy mode, less current usage
       Hysteresis disabled
       VREFDIV set to maximum to disable reference division

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA06 -  Interrupt driven GPIO, Expansion Header Pin 14, WSTK Pin 11
PD03 -  ACMP positive input, push button PB1, Expansion Header Pin 9, WSTK Pin 6

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PB01 -  ACMP positive input, push button PB0
PB02 -  LED0, WSTK Pin P17

Board:  Silicon Labs EFR32MG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48
PB01 -  ACMP positive input, push button PB0, WSTK Pin 17
PB02 -  LED0, WSTK Pin 19

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
PB01 -  ACMP positive input, push button PB1, WSTK Pin 21
PC06 -  LED0

Board:  Silicon Labs EFR32xG27 2.4 GHz 8 dBm Buck DCDC Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
PB00 -  LED0, WSTK Pin 4, Expansion Header Pin 7
PB01 -  ACMP positive input, push button PB1, WSTK Pin 6, Expansion Header Pin 9

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio
        Board (BRD4400C) + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  ACMP positive input, push button PB0, WSTK Pin 17
PB02 -  LED0, WSTK Pin 19