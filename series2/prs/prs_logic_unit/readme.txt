PRS_Logic

This project demonstrates the built-in PRS logic functions between channels.
PB0 and PB1 are set as PRS source. 

Output uses the following truth table for OR logic function:
PB1   PB0   |  PC01(OUT) 
------------------------
1     1     |     1     
1     0     |     1    
0     1     |     1     
0     0     |     0      


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Connect jumpers from P3(PC01) to P10(PB01), on WSTK Breakout Connector J101 
   to observe output of logic function on LED1 of WSTK. 
3. Build the project and download to the Starter Kit
4. Press the Push Buttons and notice how the LED1/PC01 respond.

Note: Various other logic functions like AND, NAND, NOR, XOR, NOT etc are 
available. You can switch between these logic functions by changing the 
parameters passed when calling PRS_Combine() function in main.c.

Peripherals Used:
FSRCO  - 20 MHz
PRS    - Channel 6, PB0
         Channel 7, PB1 routed to PC01(Output)

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD02   - push button PB0
PD03   - push button PB1
PC01   - PRS Channel 7 - remapped to LED1(PB01)
PRS channel 7 is not avaiblable on port A or port B. So logic function output 
on PRS channel 7 needs to be routed to port C & then remapped to LED(Port B)
