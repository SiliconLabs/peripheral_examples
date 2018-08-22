cryotimer_lfxo_em4_no_ret

This project demonstrates use of the CRYOTIMER with the low frequency
crystal oscillator (LFXO) as its clock in EM4 without GPIO retention.

After chip and DC-DC initialization, the source of the previous reset is
determined by checking the flags in the RMU_RSTCAUSE register.  If a reset
due to EM4 wake-up is detected, LED1 is turned on. LED0 then turns on to
indicate that the program is running.

Next, EM4 shutdown entry is configured so that the LFXO is kept active,
and GPIO is retention is disabled (GPIOs immediately reset upon EM4
entry).  The CRYOTIMER is configured to time out after 64K (65536) LFXO
clock periods, which is 2 seconds, cause wake-up from EM4, and request an
interrupt in all other energy modes.

With all setup tasks completed, the device enters EM2 and waits for the
CRYOTIMER interrupt.  Any LEDs that were previously turned on remain lit.
Upon time out, the CRYOTIMER interrupt is requested, waking the device
from EM2.  The interrupt handler clears the CRYOTIMER_IF_PERIOD flag and
flushes the processor core pipeline to make sure the interrupt is not
re-requested (the CRYOTIMER resides in its own, asynchronous clock
domains), and exits back to main().

Once back in main(), the device enters EM4.  Because GPIO retention is
disabled, the pins for LED0 and LED1 return to the disabled reset state,
turning off both LEDs.  After 64K (65536) LFXO periods, wake-up from EM4
is triggered, causing a restart of the processor in the same way as
power-on reset or assertion of the reset pin

How To Test:
1. Build the project and download to the Starter Kit.
2. Initially, only LED0 will turn on.
3. After 2 seconds, LED0 will turn off.
4. After another 2 seconds, if you do nothing, both LED0, indicating that
   main() has been entered, and LED1, meaning that the source of reset was
   wake-up from EM4, will be lit.
5. Both LEDs will then turn off after 2 seconds.
6. To prevent the CRYOTIMER from causing EM4 wake-up, press the RESET
   button, and you will return to step 2 above.

If desired, observe the change in current draw in Simplicity Studio's
Energy Profiler.

Modules Used:
CPU core
GPIO
HFRCO @ 19 MHz (CPU core and GPIO)
LFXO
CRYOTIMER
RMU

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
Pins Used: PF4 - LED0
           PF5 - LED1
