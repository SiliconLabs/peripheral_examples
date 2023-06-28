This project demonstrates interrupt-driven calibration of the LFRCO
against the HFXO.

The device is initialized for the board it is running on, and the
qualified LFRCO is output to a pin where it can be observed.  The
calibration mechanism is setup, and a calibration run is started.

While this demonstration code sits in a while loop doing nothing
until the "tuned" global variable goes from false to true, a real
application could perform other initialization functions that do not
require the clocks derived from the LFRCO to be stable.

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

Once tuning is complete, the BURTC is setup to request an interrupt
after about 16 seconds, the device enters EM1 to wait (so that the clock
output remains active), and the process repeats.

================================================================================

Peripherals Used:

CMU
BURTC
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

If desired, change the LFRCO_CAL.FREQTRIM bit field to a value that is
much larger or much smaller than the current value before running the
calibration routine.  This will make the change in frequency more visible
on the scope as calibration runs.

NOTE 1: While the factory-calibrated LFRCO frequency is an average of
32.768 kHz, the cycle-to-cycle deviation can be substantial.

NOTE 2: Series 2 devices support on-demand enabling of the on-chip
oscillators, like the LFRCO.  Consequently, the CMU_OscillatorEnable()
function, which is required on Series 0 and Series 1 devices, is not
used in this example.  In this case, CMU_ClkOutPinConfig() is actually
what causes the LFRCO to be started.

================================================================================

Device: EFR32MG21A020F1024IM32
Board:  Silicon Labs EFR32xG21 2.4 GHz 20 dBm Radio Board (BRD4180A) 
        + Wireless Starter Kit Mainboard (BRD4001A)
PC00 -  Expansion Header pin 4

Device: EFR32MG21A010F1024IM32
Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) 
        + Wireless Starter Kit Mainboard (BRD4001A)
PC00 -  Expansion Header pin 4

Device: EFR32ZG23B010F512IM48
Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D) 
        + Wireless Starter Kit Mainboard (BRD4001A)
PC00 -  Expansion Header pin 10

Device: EFR32FG25B222F1920IM56
Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B) 
        + Wireless Starter Kit Mainboard (BRD4001A)
PC00 -  Expansion Header pin 4, WSTK pin P1

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PC05 -  Expansion Header pin 15