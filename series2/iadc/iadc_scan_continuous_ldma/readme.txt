iadc_scan_continuous_ldma

This project demonstrates use of the IADC to continuously sample two
single-ended input channels.  The CPU starts the scan while in EM0, and
operation continues in EM2 with the LDMA saving the results to RAM and
waking the system with an interrupt request to toggle a GPIO pin.

Careful pin selection for peripherals operating in EM2 is required
because only port A and B pins remain functional; port C and D pins are
static in EM2 and cannot be used as peripheral inputs or outputs.  For
this reason, the IADC scan channel inputs in this example must be port
A/B pins.  However, any pin can be toggled after all conversion results
are transferred to RAM because the CPU is executing the LDMA IRQ handler
and the system is in EM0.

NOTE: To modify this example to take differential external
measurements, the negative inputs for scan table entries 0 and 1 must
change.  To take a differential measurement, the analog multiplexer
selection must consist of one EVEN ABUS channel and one ODD ABUS
channel.

In this example, scan table entry 0 may reference port B pin 2, so an
ODD port/pin must be selected for the IADC negative input.  Scan table
entry 1 may references port B pin 3, so the negative IADC input must be
an EVEN port/pin.  As in single-ended mode, the IADC logic will swap the
multiplexer connections to IADC input, if needed.  See reference manual
for more details.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Studio Launcher, if
   necessary.
2. Build the project and download to the Starter Kit.
3. Open the Debugger and add "scanBuffer" to the Expressions window.
4. Run the project.
5. Monitor the PC05 GPIO output on the Wireless Starter Kit, which
   toggles after each LDMA transfer sequence is completed.
6. Set a breakpoint at the end of the LDMA_IRQHandler.
7. At the breakpoint, observe the measured voltages in the Expressions
   window and how they respond to different voltage values on the
   corresponding pins.

================================================================================

Peripherals Used:
CMU     -  FSRCO @ 20 MHz
EMU
GPIO
IADC    - 12-bit resolution (2x oversampling)
        - unbuffered AVDD reference (3.3V)
        - continuous scan triggering
LDMA    - CH0
               			   
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

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB00 -  IADC input, single-ended, WSTK P15
PB01 -  IADC input, single-ended, WSTK P17
PC05 -  GPIO Push/Pull output, Expansion Header Pin 15, WSTK P12

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) +
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB00 -  IADC input, single-ended, WSTK P15
PB01 -  IADC input, single-ended, WSTK P17
PC05 -  GPIO Push/Pull output, Expansion Header Pin 15, WSTK P12