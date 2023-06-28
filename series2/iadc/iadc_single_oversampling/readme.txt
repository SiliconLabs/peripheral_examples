iadc_single_oversampling_16bit/20bit

This project demonstrates using the IADC peripheral's oversampling feature to
acquire a 16-bit/20bit resolution conversion results while operating in EM2.
IADC interrupts on conversion completion wake the MCU into EM0, where the IADC
interrupt handler converts the result to a voltage before going back into EM2.
The IADC sampling rate is 77 ksps with an oversampling rate of 32, 4.8 ksps with
an oversampling rate of 32*16 = 512, and the IADC reads GPIO pin PA05 as input. 
The PRS peripheral will output a pulse on a GPIO pin whenever the IADC finishes 
one single conversion.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary).
2. Build the project and download to the Starter Kit.
3. Open the Simplicity Debugger and add "sample" and "singleResult" to the 
   Expressions Window.
4. Apply a voltage to the IADC input pin (PA05).
5. Observe the sample field as it will display as a:
   16-bit result - "singleResult" is obtained by the formula: sample*VREF/(2^16)
   20-bit result - "singleResult" is obtained by the formula: sample*VREF/(2^20)
6. Observe OUTPUT0 toggling on an oscilloscope:
   16-bit resolution - period of 13us.
     This corresponds to sampling frequency of roughly 77 KHz
   20-bit resolution - period of 208us.
     This corresponds to sampling frequency of roughly 4.8 KHz    
7. Suspend the debugger, observe the measured voltage change in the Expressions
   Window and how it responds to different voltage values on the corresponding
   pins (see below).

================================================================================

Peripherals Used:
CMU    - FSRCO @ 20 MHz
EMU
GPIO
IADC   - 16-bit resolution, OVS 32X 
       - 20-bit resolution, OVS 512
       - Automatic Two's Complement (single-ended = unipolar) 
       - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
       - IADC and reference kept in warmup mode
       - Conversions initiated by firmware and triggered continuously
         (when a conversion completes a new one is requested immediately
         without requiring a new trigger)
PRS    - Toggles LED when IADC conversion is complete

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PA05 -  IADC input, single-ended, Expansion Header Pin 12, WSTK P9
PB01 -  GPIO Push/Pull output, Expansion Header Pin 9, WSTK Pin 6

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PA05 -  IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB01 -  GPIO Push/Pull output, WSTK Pin 17

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA05 -  IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB01 -  GPIO Push/Pull output, WSTK Pin 17

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PA05 -  IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB01 -  Push Button 1, WSTK P21

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PA05 -  IADC input, single-ended, Expansion Header pin 12, WSTK P9
PB01 -  GPIO Push/Pull output, Expansion Header pin 9, WSTK P6

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PA05 -  IADC input, single-ended, WSTK P43
PB01 -  GPIO Push/Pull output, WSTK Pin 17