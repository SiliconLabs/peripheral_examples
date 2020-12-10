This project demonstrates interrupt-driven calibration of the LFRCO
against the HFXO.

The device is initialized for the board it is running on, and the
qualified LFRCO is output to a pin where it can be observed.  The
calibration mechanism is setup, and a calibration run is started.

While this demonstration code sits in a while loop doing nothing
until the "tuned" global variable goes from false to true, a real
application could perform other initialization functions that do not
require the HFCLK/HFPERCLK to be stable.

Adjustments to the LFRCO tuning are performed in the CMU_IRQHandler()
in response to the CALRDY interrupt. So, while the calibration down
counter decrements at the HFXO frequency, the up counter counts ticks
of the LFRCO, and the CPU is freed to execute code as needed.

When the down counter reaches zero, (a) the CPU enters the
CMU_IRQHandler() in response to the CALRDY flag, (b) the up count is
compared to the ideal value, (c) the tuning value is adjusted, (d) and
the process is repeated until the ideal up count is achieved or the
tuning value from the next to last run is closer to the ideal up count
than the value from the current run.

Once tuning is complete, the CRYOTIMER is setup to request an interrupt
after about 16 seconds, the device enters EM1 to wait (so that the clock
output remains active), and the process repeats.

================================================================================

Peripherals Used:

CMU
CRYOTIMER
GPIO
EMU

================================================================================

How To Test:

1. Build the project and download to the Starter Kit (STK).
2. Click the Run/Resume button in the debugger.
3. If desired, set a breakpoint at the CMU_IRQHandler().
4. Connect an oscilloscope probe to the STK pin, listed below, on which
   the LFRCO output is driven.
5. Step through the CMU_IRQHandler() function to see how the counter
   results are compared and how adjustments are made to the LFRCO
   tuning.  After each tuning adjustment, the change in LFRCO frequency
   can be observed.

If desired, change the CMU_LFRCOCTRL_TUNING bit field to a value that
is much larger than the current value before running the calibration
routine.  This will make the change in frequency more visible on the
scope as calibration runs.

NOTE: While the factory-calibrated LFRCO frequency is an average of
32.768 kHz, the cycle-to-cycle deviation can be substantial.

================================================================================

LFRCO Output Pins

On EFM32 Jade/Pearl devices and all EFR32 devices (BG, FG, MG, and
related modules), pin PC11 is used for the HFRCO output.

On Series 1 EFM32 Tiny and Giant Gecko (EFM32TG11 and EFM32GG11/GG12),
pin PA12 is used for the HFRCO output.

PC11 and PA12 are available on the Expansion Header for each supported
board target.  Check the User's Guide for the board being used for the
specific pin assignment.