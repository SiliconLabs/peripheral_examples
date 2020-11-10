cryotimer_wakeup_msc_erase

This project shows how to use the Cryotimer with the ULFRCO to wake
up from EM4. This project also shows how to write/erase the USERDATA page.
Results of this project are shown using LEDs.

The following is what you should see upon flashing the project to the board:
1. LED0 will turn on for two seconds.
2. Both LEDs will turn off for two seconds.
3. LED1 will turn on for two seconds.
4. Both LEDS will turn off for two seconds.
5. Until a reset is triggered, go back to step 3

The following is the program flow corresponding to the LEDs:
1. Upon reset, the code initializes both LEDs off. Since the board was reset
   through powering on the board (POR), hitting the reset button (external
   reset pin), or requesting a system reset (through the debugger), th USERDATA
   page gets erased and an arbitrary value written to one word. Later, this value
   is read and if it matches the arbitrary value associated with POR/Pin/System reset
   LED0 is turned on. Because this all happens rather rapidly, it appears that LED0
   starts up on after flashing the project. Then, the board will then enter EM3 and 
   wait for the Cryotimer interrupt to go off in 2 seconds.
2. After the Cryotimer wakes us up from EM3, the board goes into EM4 and waits
   for the Cryotimer interrupt to go off in 2 seconds. Since there is no GPIO
   retention, the state of the GPIO pins for the LEDs will be reset to 0, thus
   turning both LEDs off.
3. After the Cryotimer wakes us up from EM4, the reset pin will be asserted and
   cause the program to restart. This time, however, the reset cause will come
   from waking up from EM4. The USERDATA page is erased again, and if successful,
   LED1 will be turned on. Because the USERDATA is erased, the arbitrary value no
   longer matches, and LED0 remains off.
4. Until a reset is triggered, go back to step 2

Note: the 2 second wait mentioned above is the setting by default and is
dependent on the values of the CRYOTIMER_PERIOD and CRYOTIMER_PRESCALE macros
defined in the source code. These two macros are used together to define the
duration between Cryotimer wakeup events (see the equation below).

The duration between Cryotimer wakeup events is defined to be the following
(in units of seconds):
  Time_Wakeup = (2 ^ PRESC) * (2 ^ PERIODSEL) / (F_CRYOCLK)
  F_CRYOCLK = 1000 Hz for ULFRCO
  F_CRYOCLK = 32768 Hz for LFXO and LFRCO
  
================================================================================

Note: Since this project goes into EM4, it is important to know how to unlock
the board if it ever gets stuck in EM4 and you can no longer flash the board.

One way to do this is to use the Flash Programmer tool that comes with
Simplicity Studio. Simply click "Erase" before clicking "Program"

Another way to do this is to download Simplicity Commander and then (after
connecting to the board) click "Unlock debug access" under the Flash options.

================================================================================

Peripherals Used:
ULFRCO - 1000 Hz
CRYOTIMER
RMU

================================================================================

How To Test:
0. Build the project and download it to the Starter Kit
1. Upon reset, LED0 will turn on for two seconds.
2. Both LEDs will turn off for two seconds.
3. LED1 will turn on for two seconds.
4. Both LEDS will turn off for two seconds.
5. Until a reset is triggered, go back to step 3

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF4 - LED0
PF5 - LED1
