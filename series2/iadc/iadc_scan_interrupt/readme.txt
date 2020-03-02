iadc_scan_interrupt

This project demonstrates using the IADC peripheral to take single-ended
analog measurements across two different external inputs and six of the internal
voltage supply channels. IADC interrupts handle completed conversions and store 
the converted voltage results in a global array.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "scanResult" to the Expressions Window
4. Set a breakpoint at the end of the IADC_IRQHandler (IADC_command on line 183)
5. Run the example project
6. At the breakpoint, observe the measured voltages in the Expressions Window
and how they respond to different voltage values on the corresponding pins (see below)

The IADC FIFO buffers are only four result elements deep, so the IADC scan table is set 
with only 4 channels enabled at a time to avoid FIFO buffer overflow. The IADC interrupts
on the scan table completion, and the IADC interrupt handler reads the conversions from 
the FIFO and toggles the scan table mask to enable the next set of channels in the scan table.
The scan table entry IDs are used to log results to the proper channel within the results array.
  
The first time the program halts at the breakpoint, the values in the Expression Window
will contain the first set of scan table results. The second will fill the remaining
four channels' results.

Peripherals Used:
CLK_CMU_ADC  - 20 MHz FSRCO clock for Series 2
CLK_SRC_ADC  - 10 MHz for Series 2 derived from 20MHz FSRCO clock
CLK_ADC      - 10 MHz for Series 2
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in warmup mode
			 - Conversions triggered by firmware

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25
PC05 - IADC input, single-ended, J102 of BRD4001, Pin 27

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25
PC05 - IADC input, single-ended, J102 of BRD4001, Pin 27