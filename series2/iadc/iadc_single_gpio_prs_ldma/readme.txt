iadc_single_gpio_prs_ldma

This project demonstrates using the IADC peripheral to take single-ended
analog measurements using a single external input, triggered by falling edge of
GPIO via PRS. Upon completion, an LDMA data transfer occurs storing the raw data
to memory. The IADC/LDMA operate in EM2; LDMA interrupts wake to EM0 to toggle a
GPIO before returning to EM2. Varying from the iadc_scan_gpio_prs_ldma example, 
the IADC operates on a single external input and since IADC port/pin selection
does not switch for single conversions, the input selection is not limited to 
ports A/B while operating in EM2. LDMA transfer complete toggle is performed in 
the LDMA ISR, operating in EM0, and may utilize pins on any port. 

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
3. Open the Simplicity Debugger and add "singleBuffer" to the Expressions Window
4. Set a breakpoint at the end of the LDMA_IRQHandler
*4.1 For EFR32MG21 Radio Board BRD4181A, jumper PA05 to PD02 to use pushbutton
     or toggle PA05 to trigger IADC conversion; port A/B required for PRS in EM2
5. Run the example project; press pushbutton NUM_SAMPLES to collect IADC samples
6. At the breakpoint, observe the measured voltage in the Expressions Window
and how they respond to different voltage values on the corresponding pin (see below)

Peripherals Used:
CLK_CMU_ADC  - 20 MHz FSRCO clock for Series 2
CLK_SRC_ADC  - 10 MHz for Series 2 derived from 20MHz FSRCO clock
CLK_ADC      - 10 MHz for Series 2
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference in normal mode
			 - Conversions triggered by falling GPIO via PRS
LDMA         - 32-bit transfer from IADC to buffer, loop continuously until NUM_SAMPLES
   	           are captured triggering LDMA interrupt which toggles LED

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PC05 -  IADC input, single-ended, J102->Pin 8 of BRD4001, WSTK P27
PA05 -  GPIO pull filtered input, Expansion Header Pin 12, WSTK P9; jumper to PD02
*PD02 - GPIO pull input, Expansion Header Pin 7, WSTK P4

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK P8, LED0
PC05 -  IADC input, single-ended, J102->Pin 8 of BRD4001, WSTK P27
PB00 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PC05 -  IADC input, single-ended, Expansion Header Pin 15, WSTK P12
PB01 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK