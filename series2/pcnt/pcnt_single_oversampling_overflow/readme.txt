pcnt_single_oversampling_overflow

This project demonstrates the single input oversampling mode of the pulse 
counter with interrupts. EM23GRPACLK clock is used as the pulse counter clock in
this example. The program generates an interrupt whenever the pulse counter goes 
above the configured top value. In this example, each press of Push Button PB0 
will increase the counter value by 1. The initial value of the counter is zero 
(default) and reload value for the top register is set by PCNTTopValue define.

When testing, pause the debugger and observe how the value of the counter 
register is counting up whenever BTN0 is pressed.

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

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB01 - Push Button PB0
PB02 - LED0

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24A010F1536GM48
PB01 - Push Button PB0
PB02 - LED0