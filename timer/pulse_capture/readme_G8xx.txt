pulse_capture

This readme applies for the Gecko device.

This project demonstrates single pulse capture using the TIMER module.
The HFXO, a 32 MHz oscillator, is configured to run the HFPER clock. TIMER0 is 
then initialized for input capture on Compare/Capture channel 0 for interrupt
on every second falling edge.  GPIO Pin PD1 (Expansion Header Pin 6) is to be
connected to a periodic signal or pulse generator, and two falling edges are 
captured and read from the CC0 register.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PD1
3. View the first_edge and second_edge global variables in the debugger

NOTE: The minimum duration pulse the Compare/Capture channel can accurately
measure is limited to around 700ns or 1.43 MHz. 

Peripherals Used:
HFXO  - 32 MHz
TIMER0 - CC0

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 (Expansion Header Pin 6) - Input TIM0_CC0 #3