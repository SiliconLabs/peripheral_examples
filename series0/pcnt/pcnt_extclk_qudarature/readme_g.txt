pcnt_extclk_quadrature

 This project demonstrates the external quadrature mode of the pulse counter
 peripheral. The program will request an interrupt whenever the quadrature
 decoder detects a change in direction.

For Gecko Series:

How To Test:
1. Build the project and download to the Starter Kit
2. Press BTN1, hold it
3. Press BTN0, hold it
4. Release BTN1, then release BTN0
5. Keep doing the steps above until LED0 turns on, indicating
   the inputs are synced and direction register is 1
6. If you keep on repeating the step above, LED0 will stay on
7. Now do the reverse button, press BTN0, hold; then BTN1, hold;
   release BTN0 then BTN1, press BTN0 then BTN1, when you release BTN0
   LED1 will turn on since direction changed to 0
8. If you reverse the pattern again, LED0 will turn off(toggle), note you
   need to repeat the pattern twice before noticing the change since button
   is reverse logic
9. You may also directly connect PCNT0 S0 pins and S1 pins to a function generator
   This requires modification in code where you need to remove all PRS functions
   This may also require a socket board

Peripherals Used:
PCNT

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - LED0
PC1 - LED1
PE0 - PCNT0 S0
PE1 - PCNT0 S1


