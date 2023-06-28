iadc_single_letimer_prs_ldma

This project demonstrates use of the IADC to take single-ended analog
measurements on a single channel from a periodic PRS trigger input.
Operation is in EM2 with once per second underflows of the LETIMER
triggering conversions of the selected channel and the LDMA saving
the result and requesting an interrupt after the specified number of
transfers.  The once per second LETIMER trigger pulse can be observed
on a GPIO, as can the completion of each LDMA transfer sequence with a
GPIO that drives LED0 on the Wireless Starter Kit mainboard.

Careful pin selection for peripherals operating in EM2 is required
because only port A and B pins remain functional; port C and D pins are
static in EM2 and cannot be used as peripheral inputs and outputs.  For
this reason, the IADC input and LETIMER output in this example must all
be port A/B pins.  However, when LED0 is toggled after the conversion
results are transferred to RAM, the system is in EM0 because the CPU is
executing the LDMA IRQ handler, so any GPIO pin can be used.

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
3. Open the Debugger and add "singleBuffer" to the Expressions window.
4. Set a breakpoint at the end of the LDMA_IRQHandler.
5. Run the project.
6. At the breakpoint, observe the measured voltages in the Expressions
   window and how they respond to different voltage values on the
   corresponding pin.

================================================================================

Peripherals Used:
CMU     - FSRCO @ 20 MHz, LFXO @ 32.768 kHz
IADC    - 12-bit resolution (2x oversampling)
        - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
        - PRS single conversion trigger input 
LDMA    - CH0
LETIMER - underflow output on PRS to IADC

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PA05 -  IADC input, single-ended, Expansion Header Pin 12, WSTK P9
PB00 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PB01 -  GPIO Push/Pull output, Expansion Header Pin 13, WSTK P10

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PA05 -  IADC input, single-ended, Expansion Header Pin 12, WSTK P9
PB01 -  GPIO Push/Pull output, Expansion Header Pin 9, WSTK P6
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PA05 -  IADC input, single-ended, Expansion Header Pin 7, WSTK P4
PB01 -  GPIO Push/Pull output, WSTK P17
PB02 -  GPIO Push/Pull output, WSTK P19, LED0

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) +
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA05 -  IADC input, single-ended, Expansion Header Pin 7, WSTK P4
PB01 -  GPIO Push/Pull output, WSTK P17
PB02 -  GPIO Push/Pull output, WSTK P19, LED0

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) +
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PA05 -  IADC input, single-ended, Expansion Header Pin 7, WSTK P4
PB01 -  GPIO Push/Pull output, WSTK P21
PC06 -  GPIO Push/Pull output, WSTK P27, LED0

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4191A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PA05 -  IADC input, single-ended, Expansion Header Pin 12, WSTK P9
PB00 -  GPIO Push/Pull output, WSTK P4, LED0
PB01 -  GPIO Push/Pull output, WSTK P6

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PA05 -  IADC input, single-ended, WSTK P43
PB01 -  GPIO Push/Pull output, WSTK P17
PB02 -  GPIO Push/Pull output, WSTK P19, LED0