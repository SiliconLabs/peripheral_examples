iadc_scan_ldma

This project demonstrates using the IADC peripheral in conjunction with the LDMA
to continuously sample two single-ended input channels, all while remaining in EM2.
Sample results are stored in an internal array. Sample rate is 833 ksps, and the ADC
reads GPIO pins PC04 (J102 of BRD4001, Pin 25) and PC04 (J102 of BRD4001, Pin 27)
as inputs. When the transfer is complete, WSTK LED0 is set.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "scanBuffer" to the Expressions Window
4. Monitor EXP11, LED0 on the Wireless Starter Kit and wait for it to turn on.
5. Suspend the debugger; observe the raw data array in the Expressions Window 
corresponding to the two input pins (see below)

Peripherals Used:
CLK_CMU_ADC  - 20 MHz FSRCO clock for Series 2
CLK_SRC_ADC  - 10 MHz for Series 2
CLK_ADC      - 10 MHz for Series 2
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in warmup mode 
			 - Conversions initiated by firmware and triggered continuously
			   (when a conversion completes a new one is requested immediately without 
			   requiring a new trigger)
LDMA         - 32-bit transfer from IADC to buffer, loop continuously until NUM_SAMPLES
               are captured triggering LDMA interrupt which halts IADC scan
               			   
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25
PC05 - IADC input, single-ended, J102 of BRD4001, Pin 27

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 - IADC input, single-ended, J102 of BRD4001, Pin 25
PC05 - IADC input, single-ended, J102 of BRD4001, Pin 27