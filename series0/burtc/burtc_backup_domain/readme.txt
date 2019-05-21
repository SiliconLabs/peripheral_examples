
================================================================================

This project uses the Backup Power Domain and BURTC (Backup Real Time Counter)
to maintain a counter value even during a main power brownout. The counter is
saved in the BURTC retention registers and is updated every BURTC compare0 value
(every second by default). The BURTC is used because it is one of the real time
counters still available in backup mode. This project uses the LFXO for higher
precision, but the ULFRCO can also be used to save even more energy.

================================================================================

Listed below are the boards that have a BURTC module
 - Giant Gecko (GG)
 - Leopard Gecko (LG)
 - Wonder Gecko (WG)

================================================================================

Peripherals Used:
LFXO - 32768 Hz
BURTC

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit.
2. Start the board with the Power Source Selector switch in the "DBG" position.
3. Watch the counter on the LCD display increment every second.
4. Cause a brownout by moving the power switch to the "USB" position. The board
   will turn off and the BURTC will be powered from the BU capacitor.
5. Move the power switch back to the "DBG" position. You should see that the
   counter has been maintained from when you caused the brownout and that the
   counter continues to increment.

