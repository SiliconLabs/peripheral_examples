iadc_single_interrupt

This project demonstrates using the IADC peripheral to take single-ended
analog measurements using a single external input.  After starting a
conversion, the device enters EM2 and wakes in response to the end of
conversion interrupt.

Careful pin selection for peripherals operating in EM2 is required
because only port A and B pins remain functional; port C and D pins are
static in EM2 and cannot be used as peripheral inputs and outputs.  For
this reason, the IADC scan channel inputs and LETIMER output in this
example must all be port A/B pins.  However, when LED0 is toggled after
the conversion results are transferred to RAM, the system is in EM0
because the CPU is executing the LDMA IRQ handler, so any GPIO pin can
be used.

NOTE: To modify this example to take differential external
measurements, the negative inputs for scan table entries 0 and 1 must
change.  To take a differential measurement, the analog multiplexer
selection must consist of one EVEN ABUS channel and one ODD ABUS
channel.

In this example, the single input channel is port A pin 5, so an EVEN
port/pin must be selected for the IADC negative input.  Similarly, if
port A pin 0 were to be used, the negative IADC input must be an ODD
port/pin.  As in single-ended mode, the IADC logic will swap the 
multiplexer connections to IADC input, if needed.  See the reference
manual for more details.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Studio Launcher, if
   necessary.
2. Build the project and download to the Starter Kit.
3. Open the Debugger and add "singleResult" to the Expressions window.
4. Set a breakpoint at the end of the IADC_IRQHandler.
5. Run the example project
6. At the breakpoint, observe the measured voltages in the Expressions
   window and how it responds to different voltage values on the
   corresponding pin.

================================================================================

Peripherals Used:
CMU     - FSRCO @ 20 MHz
EMU
GPIO
IADC    - 12-bit resolution (2x oversampling)
        - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
        - Conversions triggered by firmware

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PA05 -  IADC input, single-ended, Expansion Header Pin 12, WSTK P9

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PA05 -  IADC input, single-ended, Expansion Header Pin 12, WSTK P9

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PA05 -  IADC input, single-ended, Expansion Header Pin 7, WSTK P4

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) +
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA05 -  IADC input, single-ended, Expansion Header Pin 7, WSTK P4

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) +
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PA05 -  IADC input, single ended, Expansion Header Pin 7, WSTK P4

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PA05 -  IADC input, single ended, WSTK P9, Expansion Header Pin 12

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PA05 -  IADC input, single-ended, WSTK P43