iadc_scan_letimer_prs_ldma

This project demonstrates using the IADC peripheral to take single-ended 
analog measurements using two external inputs, triggered periodically by
LETIMER underflow via PRS. Upon completion, an LDMA data transfer occurs storing
the raw data to memory. The IADC/LDMA/LETIMER operate in EM2; LDMA interrupts
wake to EM0 to toggle a GPIO before returning to EM2. Due to the IADC operating 
in scan mode, external inputs have been selected from ports A/B as these ports
remain fully functional in EM2 and the analog multiplexers can switch between
external inputs during the scan. GPIO is also toggled with LETIMER underflow via
LETIMER output, and thus must be routed to port A/B pins when operating in EM2. 
LDMA transfer complete toggle is performed in the LDMA ISR, operating in EM0, 
and may utilize pins on any port.  

Note: To utilize differential-ended analog measurements for the external inputs,
the negative inputs for scan table entries 0 and 1 must be modified for an
external port/pin. Analog multiplexer selection must consist of one EVEN ABUS
selection and one ODD ABUS selection for differential mode to operate correctly: 
*For scan table entry 0 which references Port B Pin 0, an ODD Port/Pin selection
must be used for IADC negative input.
*For scan table entry 1 which references Port B Pin 1, an EVEN Port/Pin
selection must be used for IADC negative input. 
As with singled-ended mode, the IADC logic will automatically swap the multiplexer
connections to IADC input if needed. See reference manual for more details. 

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() have no
effect as oscillators are automatically turned on/off based on demand from the peripherals;
CMU_ClockEnable() is a dummy function for EFR32xG21 for library consistency/compatibility.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "scanBuffer" to the Expressions Window
4. Set a breakpoint at the end of the LDMA_IRQHandler
5. Run the example project
6. At the breakpoint, observe the measured voltage in the Expressions Window
and how they respond to different voltage values on the corresponding pin (see below)

The IADC interrupts on conversion completion, wakes the MCU into EM0 where the 
IADC interrupt handler converts the result to a voltage before returning to EM2.
IADC raw conversion data is stored in global variable "sample" and voltage
conversion is stored in global variable "scanResult".

Peripherals Used:
CLK_CMU_ADC  - 20 MHz FSRCO clock for Series 2
CLK_SRC_ADC  - 10 MHz for Series 2 derived from 20MHz FSRCO clock
CLK_ADC      - 10 MHz for Series 2
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference in normal mode
			 - Conversions triggered by LETIMER via PRS
LDMA         - 32-bit transfer from IADC to buffer, loop continuously until NUM_SAMPLES
   	           are captured triggering LDMA interrupt which toggles LED
LETIMER      - 32768 Hz LFXO sourcing 1 Hz LETIMER interrupt

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  IADC input, single-ended, Expansion Header Pin 11, WSTK P8
PB01 -  IADC input, single-ended, Expansion Header Pin 13, WSTK P10
PA05 -  GPIO Push/Pull output, Expansion Header Pin 12, WSTK P9
PD02 -  GPIO Push/Pull output, Expansion Header Pin 7, WSTK P4

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB00 -  IADC input, single-ended, Expansion Header Pin 7, WSTK P4
PB01 -  IADC input, single-ended, Expansion Header Pin 9, WSTK P6
PA05 -  GPIO Push/Pull output, Expansion Header Pin 12, WSTK P9
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB00 -  IADC input, single-ended, WSTK P15
PB01 -  IADC input, single-ended, WSTK P17
PA05 -  GPIO Push/Pull output, Expansion Header Pin 7, WSTK P4
PD02 -  GPIO Push/Pull output, Expansion Header Pin 9, WSTK P6