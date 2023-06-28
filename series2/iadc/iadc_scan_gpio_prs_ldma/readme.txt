iadc_scan_gpio_prs_ldma

This project demonstrates use of the IADC to take single-ended analog
measurements using two external inputs, triggered via the PRS in response
to a positive (rising) edge on a user-specified GPIO pin.  Operation is in EM2
with the GPIO positive edge triggering a scan of the selected channels and the
IADC saving those result and requesting an interrupt when all transfers
are complete.  After the specified number of samples are taken in
response to the GPIO triggers, a GPIO output is toggled on the Wireless Starter
Kit mainboard.

Careful pin selection for peripherals operating in EM2 is required
because only port A and B pins remain functional; port C and D pins are
static in EM2 and cannot be used as peripheral inputs or outputs.  For
this reason, the IADC scan channel inputs and GPIO trigger input in this
example must all be port A/B pins.  However, LDMA transfer complete toggle is
performed in the LDMA ISR, operating in EM0, and may utilize pins on any port.

NOTE: To modify this example to take differential external
measurements, the negative inputs for scan table entries 0 and 1 must
change.  To take a differential measurement, the analog multiplexer
selection must consist of one EVEN ABUS channel and one ODD ABUS
channel.

In this example, scan table entry 0 may reference port B pin 2, so an
ODD port/pin must be selected for the IADC negative input.  Scan table
entry 1 may reference port B pin 3, so the negative IADC input must be
an EVEN port/pin.  As in single-ended mode, the IADC logic will swap the
multiplexer connections to IADC input, if needed.  See the reference
manual for more details.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Studio Launcher, if
   necessary.
2. Build the project and download to the Starter Kit.
3. Open the Debugger and add "scanBuffer" to the Expressions window.
4. On BRD4181A for EFR32xG21 only, jumper PA05 to PD02 in order to use
   push button 0 to trigger each scan sequence.  Alternatively, toggle
   PA05 low then high repeatedly to generate the falling edge triggers.
5. Set a breakpoint at the end of the LDMA_IRQHandler.
6. Run the project.
7. Press push button 0 (or toggle PA05 on EFR32xG21 BRD4181A) to trigger
   a scan conversion sequence.  Do this 5 times to collect 10 (the
   default for NUM_SAMPLES) total IADC samples.
8. At the breakpoint, observe the conversion results in the Expressions
   window and how they respond to different voltage values on the
   corresponding pins.

================================================================================

Peripherals Used:
CMU     - FSRCO @ 20 MHz
EMU
GPIO
IADC    - 12-bit resolution (2x oversampling)
        - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
        - PRS scan trigger input 
LDMA    - CH0

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  IADC input, single-ended, Expansion Header Pin 11, WSTK P8
PB01 -  IADC input, single-ended, Expansion Header Pin 13, WSTK P10
PA05 -  GPIO input, pull-up, filtered, Expansion Header Pin 12 (WSTK P9),
        jumper to Expansion Header Pin 7 (PD02, WSTK P4)
PD03 -  GPIO push/pull output, Expansion Header Pin 9, WSTK P6

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB02 -  IADC input, single-ended, Expansion Header Pin 15, WSTK P12
PB03 -  IADC input, single-ended, Expansion Header Pin 16, WSTK P13
PB00 -  GPIO input, pull-up, filtered, Expansion Header Pin 7, WSTK P4,
        Push Button 0
PD03 -  GPIO Push/Pull output, Expansion Header Pin 13, WSTK P10, LED1

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 -  IADC input, single-ended, WSTK P19
PB03 -  IADC input, single-ended, WSTK P21
PB01 -  GPIO input, pull-up, filtered, WSTK P17, Push Button 0
PD03 -  GPIO Push/Pull output, WSTK P26, LED1

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) +
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02 -  IADC input, single-ended, WSTK P19
PB03 -  IADC input, single-ended, WSTK P21
PB01 -  GPIO input, pull-up, filtered, WSTK P17, Push Button 0
PB04 -  GPIO Push/Pull output, WSTK P26, LED1

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) +
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB02 -  IADC input, single-ended, Expansion Header Pin 15, WSTK P12
PB03 -  IADC input, single-ended, Expansion Header Pin 16, WSTK P13
PB00 -  GPIO input, pull-up, filtered, WSTK P17, Push Button 0
PC07 -  GPIO Push/Pull output, WSTK P26, LED1

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB02 -  IADC input, single-ended, Expansion Header Pin 15, WSTK P12
PB03 -  IADC input, single-ended, Expansion Header Pin 16, WSTK P13
PB00 -  GPIO input, pull-up, filtered, WSTK P4, Expansion Header Pin 7, Push Button 0
PB01 -  GPIO Push/Pull output, WSTK P6, Expansion Header Pin 9, LED1

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 -  IADC input, single-ended, WSTK P19
PB03 -  IADC input, single-ended, WSTK P21
PB01 -  GPIO input, pull-up, filtered, WSTK P17, Push Button 0
PD03 -  GPIO Push/Pull output, WSTK P23, LED1
