pcnt_extclk_quadrature

This project demonstrates the external quadrature mode of the pulse counter
peripheral. The program will request an interrupt whenever the quadrature
decoder detects a change in direction.

When testing, pause the debugger and observe how the value of the counter 
register is counting up whenever using the BTN0 -> BTN1 sequence. Notice no
change in counter value when using the BTN1 -> BTN0 sequence, as this example
is configured to only count up (and due to the input configuration of the PCNT
peripheral, upward counting corresponds to the BTN0 -> BTN1 sequence).

Use Simplicity Studio's Energy Profiler to observe current consumption while
example runs in low energy mode.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Press BTN1, hold it
3. Press BTN0, hold it
4. Release BTN1, then release BTN0
5. Keep doing the steps above until LED0 turns on, indicating
   the inputs are synced. LED0 turns on to indicate change in Status register's
   direction bit to 1 (down counter; PCNT initialized counting up)
6. If you continue repeating steps 2-4 above, LED0 will remain on
7. Now do the reverse button sequence: press BTN0, hold; then BTN1, hold;
   release BTN0 then BTN1, press BTN0 then BTN1. When BTN0 is released
   LED1 will turn on since Status register's direction bit changed to 0 (up 
   counter)
8. If you reverse the pattern again, LED0 will turn off (toggle); note you
   need to repeat the pattern twice before noticing the change because the push
   button behavior is reverse logic (pressed is logic low)
9. You may also directly connect PCNT0 S0 pins and S1 pins to a function 
   generator; This requires modification in code where you need to remove all 
   PRS functions, and may also require a socket board.

================================================================================

Peripherals Used:
CMU   - LFRCO @ 32768 Hz - On-demand clock used temporarily for configuring PCNT
External Clock - Push Button 0
PCNT

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  Push Button PB0
PB02 -  LED0
PB03 -  Push Button PB1
PD03 -  LED1

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  Push Button PB0
PB02 -  LED0
PB03 -  Push Button PB1
PB04 -  LED1

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  Push Button PB0
PC06 -  LED0
PB01 -  Push Button PB1
PC07 -  LED1

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  Push Button PB0
PB02 -  LED0
PB03 -  Push Button PB1
PD03 -  LED1