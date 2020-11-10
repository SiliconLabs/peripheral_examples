prs_logic_unit

This project demonstrates the built-in PRS logic functions between channels.
PB0 and PB1 are set as PRS source. 

Output uses the following truth table for OR logic function:
PB1   PB0   |  OUTPUT 
------------------------
1     1     |     1     
1     0     |     1    
0     1     |     1     
0     0     |     0  

When the push button (PB0 or PB1) is pressed, a logical low is sampled by the
GPIO and sent to the corresponding PRS channel

When the push button (PB0 or PB1) is released, a logical high is sampled by the
GPIO and sent to the corresponding PRS channel

A logical low output will have the LED off and a logical high output will have
the LED on.


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. For EFR32xG21 device: Connect jumpers from P3 (PC01) to P10 (PB01), on WSTK 
   Breakout Connector J101 to observe output of logic function on LED1 of WSTK.
   Skip this step if not using EFR32xG21 device.
3. Build the project and download to the Starter Kit
4. Press the Push Buttons and notice how the LED1/PC01 responds.

Note: Various other logic functions like AND, NAND, NOR, XOR, NOT etc are 
available. You can switch between these logic functions by changing the 
parameters passed when calling PRS_Combine() function in main.c.

Note: For EFR32xG22 devices, you may choose any asynchronous PRS channel as
your input B. For EFR32xG21 devices, you have to choose (input A - 1) as your
input B. This is reflected on the PRS channel selection in the source file


Peripherals Used:
FSRCO  - 20 MHz
PRS (EFR32xG21)  - Channel 6, PB0
                   Channel 7, PB1 routed to PC01 (Output)
PRS (EFR32xG22)  - Channel 1, PB0                   
                   Channel 6, PB1 routed to PD03 (Output)
                  
                  
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD02  - push button PB0
PD03  - push button PB1
PC01  - PRS Channel 7 - remapped to LED1 (PB01)
PRS channel 7 is not avaiblable on port A or port B. So logic function output 
on PRS channel 7 needs to be routed to port C & then remapped to LED (Port B)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB00  - push button PB0
PB01  - push button PB1
PD03  - PRS Channel 6
