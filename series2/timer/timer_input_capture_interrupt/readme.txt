
================================================================================

This project demonstrates the use of the TIMER module for interrupt
based input capture. After button 0 is pressed, the PRS routes this signal
to the timer to indicate that an input capture needs to occur. An interrupt
then occurs because the Compare/Capture channel interrupt flag is set.
The captured timer value is then stored in the user's buffer.

Note: This project triggers a capture on a falling edge because the buttons
are negative logic (i.e. the pin value goes low when the button is pressed).

The example also has comments that show how polled mode can be used for input 
capture.

================================================================================

Peripherals Used:
TIMER0 - HFPERclk
PRS    - (route GPIO signal to the timer)

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Go into debug mode and click run
3. Press button 0 to trigger the input capture and have the value be recorded
4. Pause the debugger to check if the value was recorded in the global
   `buffer` variable

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PD2 - Push Button 0

