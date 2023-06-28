timer_input_capture_interrupt

This project demonstrates use of the TIMER module for interrupt-driven
input capture.  Button 0 is presses are captured and stored in a buffer
in the TIMER interrupt handler.

Note: This example captures falling edge because one side of each
STK push button switch is grounded while the other is intended to be
pulled high by the GPIO pin to which it is connected.

Comments are provided in the example that explain how to convert the
code from interrupt-driven to polled operation.

Note: On EFR32xG21 devices, calls to CMU_ClockEnable() have no effect
as clocks are automatically turned on/off in response to on-demand
requests from the peripherals. CMU_ClockEnable() is a dummy function
on EFR32xG21 present for software compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Wireless Starter Kit.
2. Go into debug mode and click Run.
3. Press button 0 to trigger the input capture and have the value recorded.
4. Pause the debugger, add the buffer[] variable to the Expressions
   pane, and expand the array to see each the value of the counter for
   each edge (button press) captured.

================================================================================

Peripherals Used:
CMU    - HFRCO @ 19 MHz
TIMER0 - EM01GRPACLK

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PD02  - Push Button 0

Board:  Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A)
        + Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB00  - Push Button 0

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01  - Push Button 0

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01  - Push Button 0

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
PB00  - Push Button 0

Board:  Silicon Labs EFR32MG27 Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
PB00  - Push Button 0

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C)
        + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01  - Push Button 0