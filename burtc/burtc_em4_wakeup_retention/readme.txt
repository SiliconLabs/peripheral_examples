
================================================================================

This project uses the BURTC (Backup Real Time Counter) to wake the board up from
EM4 mode and thus trigger a reset. This project also shows how to use the BURTC
retention registers to have data persist between resets. The LCD will show
the number of resets triggered by the BURTC.

Note: The SysTick and LCD functions are not neccessary for accessing the BURTC
retention registers. They are only included to help the user see that the
retention registers are actually working.

Note: Since this project goes into EM4, it is important to know how to unlock
the board if it ever gets stuck in EM4 and you can no longer flash the board.
The easiest way to do this is to download Simplicity Commander and then (after
connecting to the board) click "Unlock debug access" under the Flash options.

================================================================================

Listed below are the boards that have a BURTC module
 - Giant Gecko (GG)
 - Leopard Gecko (LG)
 - Wonder Gecko (WG)

================================================================================

Peripherals Used:
ULFRCO - 1000 Hz
BURTC

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Watch the LCD display the number of resets triggered by the BURTC

