iadc_scan_interrupt

This project demonstrates use of the IADC to take single-ended analog
measurements across two different external inputs and six of the
internal voltage supply channels.  The IADC operates in EM2 and wakes
the device to save the completed conversion results, convert them to
volts, and store them in a global array.  Because the example runs in
EM2, only ports A and B remain fully functional and can be used for
the external scan inputs.

NOTE: To modify this example to take differential external
measurements, the negative inputs for scan table entries 0 and 1 must
change.  To take a differential measurement, the analog multiplexer
selection must consist of one EVEN ABUS channel and one ODD ABUS
channel.

In this example, scan table entry 0 references port B pin 0, so an ODD
port/pin must be selected for the IADC negative input.  Scan table
entry 1 references port B pin 1, so the negative IADC input must be an
EVEN port/pin.  As in single-ended mode, the IADC logic will swap the
multiplexer connections to IADC input, if needed.  See reference manual
for more details.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Studio Launcher, if
   necessary.
2. Build the project and download to the Starter Kit.
3. Open the Debugger and add "scanResult" to the Expressions window.
4. Set a breakpoint at the end of the IADC_IRQHandler.
5. Run the project.
6. At the breakpoint, observe the measured voltages in the Expressions
   window and how they respond to different voltage values on the
   corresponding pins and external supply inputs (see below).

Because IADC FIFO depth varies across Series 2 EFM32/EFR32 devices, the
IADC scan table is configued to process 4 channels enabled at a time to
avoid FIFO overflow.  The IADC interrupts on scan table completion, and
the IRQ handler reads the conversion results from the FIFO and swaps the
mask to alternate between each set of four channels in the scan table.
The scan table entry IDs are used to save the results to the proper
channel entry within the results array.

The first time the program halts at the breakpoint, the values in the
Expression window will contain the first set of scan table results. The
second time through will fill the remaining four channels' results.

================================================================================

Peripherals Used:
CMU     - FSRCO @ 20 MHz
EMU
GPIO
IADC    - 12-bit resolution (2x oversampling)
        - unbuffered AVDD reference (3.3V)

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  IADC input, single-ended, Expansion Header Pin 11, WSTK P8
PB01 -  IADC input, single-ended, Expansion Header Pin 13, WSTK P10

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB00 -  IADC input, single-ended, Expansion Header Pin 7, WSTK P4
PB01 -  IADC input, single-ended, Expansion Header Pin 9, WSTK P6

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) +
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB00 -  IADC input, single-ended, WSTK P15
PB01 -  IADC input, single-ended, WSTK P17