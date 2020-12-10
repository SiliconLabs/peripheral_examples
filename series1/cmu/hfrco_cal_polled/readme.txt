This project demonstrates polled calibration of the HFRCO against
the LFXO.

The device is initialized for the board it is running on, and the
qualified HFRCO is output to a pin where it can be observed.  The
CRYOTIMER is setup to generate an interrupt every 16 seconds, and the
device enters EM1 (so that the clock output remains active).

Upon wake, the standard polling mechanism is used to calibrate the
HFRCO against the LFXO with the CMU providing the necessary hardware
assistance via its up and down counters.  The process repeats every
16 seconds.

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
3. If desired, set a breakpoint at the calHFRCO(19000000) function call
   in the main loop.
4. Connect an oscilloscope probe to the STK pin, listed below, on which
   the HFRCO output is driven.
5. Step through the calHFRCO() function to see how the counter results
   are compared and how adjustments are made to the HFRCO tuning.  After
   each tuning adjustment, the change in HFRCO frequency can be observed.

If desired, change the CMU_HFRCOCTRL_TUNING bit field to a value that
is much larger than the current value before running the calibration
routine.  This will make the change in frequency more visible on the
scope as calibration runs.

================================================================================

HFRCO Output Pins

On EFM32 Jade/Pearl devices and all EFR32 devices (BG, FG, MG, and
related modules), pin PC11 is used for the HFRCO output.

On Series 1 EFM32 Tiny and Giant Gecko (EFM32TG11 and EFM32GG11/GG12),
pin PA12 is used for the HFRCO output.

PC11 and PA12 are available on the Expansion Header for each supported
board target.  Check the User's Guide for the board being used for the
specific pin assignment.