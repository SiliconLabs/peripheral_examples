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

How To Test:
1. Build the project and download to the Starter Kit
2. Press Button 0 once to escape the debugging catch
3. Press Button 0 six times
4. Observe LED0 toggle
5. Repeat steps 3 thru 5

Peripherals Used:
LFRCO - 32768 Hz
PCNT

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB1 - Push Button PB0
PB2 - LED0