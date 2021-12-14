pcnt_oversampling_quadrature

This project demonstrates the oversampling quadrature decoder mode of the pulse 
counter peripheral. The program will request an interrupt whenever the 
quadrature decoder detects a change in direction or a counter overflow or 
counter underflow. The example utilizes OVSQUAD 4X mode, such that counter value
changes with each state transition of S1IN/S0IN (see PCNT State Transitions in 
Reference Manual). LED0 toggles on detected direction changes, and LED1 toggles 
on counter overflow/underflow events. Optional pulse width filter and flutter 
removal are both enabled.

When testing, pause the debugger and observe how the value of the counter 
register is counting up/down depending on the PCNT State Transitions using 
BTN0/BTN1.

Use Simplicity Studio's Energy Profiler to observe current consumption while
example runs in low energy mode.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Run in the debugger or just the board 
NOTE: Due to WSTK schematic, both buttons released starts PCNT State Transition
      in S2: 'b11 - See PCNT State Transition diagram in Reference Manual
3. Press BTN0, hold it
4. Press BTN1, hold it
5. Release BTN0, release BTN1
6. Repeat steps 3-5 for a total of 11 press/releases (presses + releases = 11)
7. Observe LED1 toggles on to indicate an overflow; with hysteresis, count is 
   reloaded with TOP/2 = 5 for this example
8. Repeat steps 3-5 for a total of 6 presses/releases (presses + releases = 6)
9. Observe LED1 toggles on to indicate an overflow; with hysteresis, count is 
   reloaded with TOP/2 = 5 for this example
10.Now do the reverse button sequence: press BTN1, hold; then BTN0, hold;
   release BTN1 then BTN0, press BTN1 then BTN0, when you release BTN0
   LED0 will turn on since Status register's direction bit changed to 0 (up 
   counter)
11.If you reverse the pattern again, LED0 will turn off (toggle); note you
   need to repeat the pattern twice before noticing the change because the push
   button behavior is reverse logic (pressed is logic low)
12.You may set a break point in the interrupt service routine and step through 
   execution to get the current absolute count based on OF/UF events and the TOP
   value/hysteresis.

================================================================================

Peripherals Used:
CMU  - LFRCO @ 32768 Hz - Used temporarily for configuring PCNT peripheral
External Clock - Push Button 0
PCNT

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB01 - Push Button PB0
PB02 - LED0
PB03 - Push Button PB1
PD03 - LED1

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24A010F1536GM48
PB01 - Push Button PB0
PB02 - LED0
PB03 - Push Button PB1
PB04 - LED1