pcnt_single_oversampling_underflow

This project demonstrates the single input oversampling mode of the pulse 
counter with interrupts. EM23GRPACLK clock is used as the pulse counter clock in
this example. The program generates an interrupt whenever the pulse counter goes 
below zero. In this example, each press of Push Button PB0 will decrease the 
counter value by 1. The initial value of the counter and the reload value for 
the top register is set by PCNTTopValue define.

When testing, pause the debugger and observe how the value of the counter 
register is counting down whenever BTN0 is pressed.

Use Simplicity Studio's Energy Profiler to observe current consumption while
example runs in low energy mode.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Press Button 0 six times
3. Observe LED0 toggle
4. Repeat steps 2 thru 4

================================================================================

Peripherals Used:
CMU  - LFRCO @ 32768 Hz
PCNT

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  Push Button PB0
PB02 -  LED0

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  Push Button PB0
PB02 -  LED0

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  Push Button PB0
PC06 -  LED0

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  Push Button PB0
PB02 -  LED0