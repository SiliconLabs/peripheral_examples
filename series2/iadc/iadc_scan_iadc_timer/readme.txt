iadc_scan_iadc_timer

This project demonstrates use of the IADC to take single-ended analog
analog measurements of two input channels.  The IADC operates in EM0
with its local timer triggering scan sequences.  An interrupt is
requested when the specified FIFO data valid level is reached, and the
conversion results are converted to volts and stored in a global array.
A GPIO is toggled during the IADC interrupt handler to signal the end
of the most recent scan.

NOTE: To modify this example to take differential external
measurements, the negative inputs for scan table entries 0 and 1 must
change.  To take a differential measurement, the analog multiplexer
selection must consist of one EVEN ABUS channel and one ODD ABUS
channel.

In this example, scan table entry 0 references port B pin 0, so an
ODD port/pin must be selected for the IADC negative input.  Scan table
entry 1 references port B pin 1, so the negative IADC input must be
an EVEN port/pin.  As in single-ended mode, the IADC logic will swap the
multiplexer connections to IADC input, if needed.  See reference manual
for more details.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Studio Launcher, if
   necessary.
2. Build the project and download it to the Starter Kit.
3. Open the Debugger and add "scanResult" to the Expressions window.
4. Using an oscilloscope, monitor the GPIO output, which toggles after
   each scan is complete (every 1 ms).
5. Set a breakpoint at the end of the IADC_IRQHandler.
6. At the breakpoint, observe the measured voltages in the Expressions
   window and how they respond to different voltage values on the
   corresponding pins (see below).

================================================================================

Peripherals Used:
CMU     - HFXO
GPIO
IADC    - 12-bit resolution (2x oversampling)
        - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
        - local timer triggering at 1 kHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  IADC input, single-ended, Expansion Header Pin 11, WSTK P8
PB01 -  IADC input, single-ended, Expansion Header Pin 13, WSTK P10
PC05 -  GPIO Push/Pull output, WSTK P27

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB00 -  IADC input, single-ended, Expansion Header Pin 7, WSTK P4
PB01 -  IADC input, single-ended, Expansion Header Pin 9, WSTK P6
PC05 -  GPIO Push/Pull output, WSTK P27

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB00 -  IADC input, single-ended, WSTK P15
PB01 -  IADC input, single-ended, WSTK P17
PC05 -  GPIO Push/Pull output, Expansion Header Pin 15, WSTK P12

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) +
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB00 -  IADC input, single-ended, WSTK P15
PB01 -  IADC input, single-ended, WSTK P17
PC05 -  GPIO Push/Pull output, Expansion Header Pin 15, WSTK P12

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) +
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  IADC input, single-ended, WSTK P17
PB01 -  IADC input, single-ended, WSTK P21
PC05 -  GPIO Push/Pull output, WSTK P33

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 -  IADC input, single-ended, WSTK P4, Expansion Header 7
PB01 -  IADC input, single-ended, WSTK P6, Expansion Header 9
PC05 -  GPIO Push/Pull output, WSTK P27

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB00 -  IADC input, single-ended, WSTK P15
PB01 -  IADC input, single-ended, WSTK P17
PC05 -  GPIO Push/Pull output, WSTK P12, Expansion Header 15