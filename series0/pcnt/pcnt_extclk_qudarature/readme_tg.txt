pcnt_extclk_quadrature

 This project demonstrates the external quadrature mode of the pulse counter
 peripheral. The program will request an interrupt whenever the quadrature
 decoder detects a change in direction.

For Tiny Gecko Series:

How To Test:
1. Build the project and download to the Starter Kit
2. Run in the debugger 
3. Press BTN1, hold it
4. Press BTN0, hold it
5. Release BTN1, then release BTN0
6. Keep doing the steps above until LED0 turns on, indicating
   the inputs are synced and direction register is 1
7. If you keep on repeating the step above, LED0 will stay on
8. Now do the reverse button, press BTN0, hold; then BTN1, hold;
   release BTN0 then BTN1, press BTN0 then BTN1, when you release BTN0
   LED1 will turn on since direction changed to 0
9. If you reverse the pattern again, LED0 will turn off(toggle), note you
   need to repeat the pattern twice before noticing the change since button
   is reverse logic
10. You may also directly connect PCNT0 S0 pins and S1 pins to a function generator
   This requires modification in code where you need to remove all PRS functions
   This may also require a socket board

Peripherals Used:
PCNT

Board:  Silicon Labs EFM32TG Starter Kit (STK3300)
Device: EFM32TG840F32
PD7 - LED0
PD8 - Push Button PB0
PB11 - Push Button PB1
