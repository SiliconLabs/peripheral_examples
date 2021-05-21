iadc_single_interrupt

This project demonstrates using the IADC peripheral to take single-ended
analog measurements using a single external input. The IADC operates in EM2 and
IADC interrupts wake to EM0 to handle completed conversions and store the 
converted voltage result in a global variable. Varying from the 
iadc_scan_interrupt example, the IADC operates on a single external input, and
since IADC port/pin selection does not switch for single conversions, the input
selection is not limited to ports A/B while operating in EM2. 

Note: To utilize differential-ended analog measurements for the external input,
the negative input for single input must be modified for an external port/pin. 
Analog multiplexer selection must consist of one EVEN ABUS selection and one ODD
ABUS selection for differential mode to operate correctly: 
*For single input which references Port C Pin 5, an EVEN Port/Pin selection
must be used for IADC negative input.
As with singled-ended mode, the IADC logic will automatically swap the multiplexer
connections to IADC input if needed. See reference manual for more details. 

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() have no
effect as oscillators are automatically turned on/off based on demand from the peripherals;
CMU_ClockEnable() is a dummy function for EFR32xG21 for library consistency/compatibility.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "singleResult" to the Expressions Window
4. Set a breakpoint at the end of the IADC_IRQHandler (IADC_command)
5. Run the example project
6. At the breakpoint, observe the measured voltage in the Expressions Window
and how they respond to different voltage values on the corresponding pin (see below)

The IADC interrupts on conversion completion, wakes the MCU into EM0 where the 
IADC interrupt handler converts the result to a voltage before returning to EM2.
IADC raw conversion data is stored in global variable "sample" and voltage
conversion is stored in global variable "singleResult".

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
PC05 -  IADC input, single-ended, WSTK P27

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PC05 -  IADC input, single-ended, WSTK P27

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PC05 -  IADC input, single-ended, Expansion Header Pin 15, WSTK P12