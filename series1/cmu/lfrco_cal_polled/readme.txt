lfrco_cal_polled

This project demonstrates calibration of the LFRCO against the HFXO.

The device is initialized for the board it is running on, and the
qualified LFRCO is output to a pin where it can be observed.  The
CRYOTIMER is setup to generate an interrupt every 16 seconds, and the
device enters EM2.

Upon wake, the standard polling mechanism is used to calibrate the
LFRCO against the HFXO with the CMU providing the necessary hardware
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
3. If desired, set a breakpoint at the calLFRCO(32768) function call in
   the main loop.
4. Connect an oscilloscope probe to the STK pin on which, listed below,
   on which the LFRCO output is driven.
5. Step through the calLFRCO() function to see how the counter results
   are compared and how adjustments are made to the LFRCO tuning.  After
   each tuning adjustment, the change in LFRCO frequency can be observed.

If desired, change the CMU_LFRCOCTRL_TUNING bit field to a value that is
much larger or much smaller than the current value before running the
calibration routine.  This will make the change in frequency more visible
on the scope as calibration runs.


================================================================================

LFRCO Output Pins

On EFM32 Jade/Pearl devices and all EFR32 devices(BG, FG, MG, and related
modules), pin PC11 is used for the LFRCO output.

On Series 1 EFM32 Tiny and Giant Gecko (EFM32TG11 and EFM32GG11/GG12),
pin PC12 is used for the LFRCO output.