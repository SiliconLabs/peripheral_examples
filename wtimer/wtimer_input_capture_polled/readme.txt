
================================================================================

This project demonstrates the use of the WTIMER module for polled
input capture. The Compare/Capture channel interrupt flag is polled until
Push Button 0 is pressed. The PRS then routes this signal to the timer to
indicate that an input capture needs to occur. The captured timer value is
then stored in the user's buffer.

Note: This project triggers a capture on a falling edge because the buttons
are negative logic (i.e. the pin value goes low when the button is pressed).

Note: only the following boards have a WTIMER module
 - PG12
 - TG11
 - GG11
 - radio boards series 12, 13, 14

================================================================================

Peripherals Used:
HFRCO   - 19 MHz
WTIMER0 - CC0
PRS	    - (route GPIO signal to the timer)

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Go into debug mode and click run
3. Press button 0 to trigger the input capture and have the value be recorded
4. Pause the debugger to check if the value was recorded in the buffer

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF6 - Push Button 0

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PF6 - Push Button 0

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PF6 - Push Button 0

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PF6 - Push Button 0

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PF6 - Push Button 0

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PF6 - Push Button 0

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PF6 - Push Button 0

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PF6 - Push Button 0

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PF6 - Push Button 0

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PF6 - Push Button 0

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC8 - Push Button 0

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD5 - Push Button 0

