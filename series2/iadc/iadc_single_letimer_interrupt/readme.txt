iadc_single_letimer_interrupt

This project demonstrates using the IADC peripheral to take single-ended
analog measurements using a single external input, triggered periodically by 
LETIMER underflow interrupt. The IADC/LETIMER operate in EM2; LETIMER interrupts
wake to EM0 to trigger IADC to start a single conversion, and IADC interrupts
wake to EM0 to handle completed conversions and store the converted voltage
result in a global variable. Since the IADC conversion is done in EM2, only
Port A/B pins can be used. GPIO are toggled with LETIMER underflow and IADC
conversion complete. LETIMER GPIO is toggled through LETIMER output, 
and thus must be routed to port A/B pins when operating in EM2. IADC
conversion complete toggle is performed in the IADC ISR, thus operating in EM0,
and may utilize pins on any port. 

Note: To utilize differential-ended analog measurements for the external input,
the negative input for single input must be modified for an external port/pin. 
Analog multiplexer selection must consist of one EVEN ABUS selection and one ODD
ABUS selection for differential mode to operate correctly: 
*For single input which references Port A Pin 5, an EVEN Port/Pin selection
must be used for IADC negative input.
As with singled-ended mode, the IADC logic will automatically swap the 
multiplexer connections to IADC input if needed. See reference manual for more 
details. 

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "singleResult" to the Expressions Window
4. Set a breakpoint at the end of the IADC_IRQHandler (IADC_command)
5. Run the example project
6. At the breakpoint, observe the measured voltage in the Expressions Window
   and how they respond to different voltage values on the corresponding pin 
   (see below)

The IADC interrupts on conversion completion, wakes the MCU into EM0 where the 
IADC interrupt handler converts the result to a voltage before returning to EM2.
IADC raw conversion data is stored in global variable "sample" and voltage
conversion is stored in global variable "singleResult".

================================================================================

Peripherals Used:
CMU     - FSRCO @ 20 MHz, LFXO @ 32.768 kHz
EMU
GPIO
IADC    - 12-bit resolution, 
        - Automatic Two's Complement (single-ended = unipolar) 
        - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
        - IADC and reference in normal mode
        - Conversions triggered by firmware
LETIMER - 1 Hz LETIMER interrupt

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PA05 -  IADC input, single-ended, Expansion Header Pin 12, WSTK P9
PB01 -  GPIO Push/Pull output, Expansion Header Pin 13, WSTK P10, LED1

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PA05 -  IADC input, single-ended, Expansion Header Pin 12, WSTK P9
PB01 -  GPIO Push/Pull output, Expansion Header Pin 9, WSTK P6

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PA05 -  IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB01 -  GPIO Push/Pull output, WSTK P17

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PA05 -  IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB01 -  GPIO Push/Pull output, WSTK P17

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06 -  GPIO Push/Pull output, WSTK P27, LED0
PA05 -  IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB01 -  GPIO Push/Pull output, WSTK P21

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00  - GPIO Push/Pull output, WSTK P4, LED0
PA05  - IADC input, single-ended, Expansion Header pin 12, WSTK P9
PB01 -  GPIO Push/Pull output, WSTK P6, LED1

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PA05 -  IADC input, single-ended, WSTK P43
PB01 -  GPIO Push/Pull output, WSTK P17