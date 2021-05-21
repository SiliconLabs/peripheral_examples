iadc_scan_continuous_ldma

This project demonstrates using the IADC peripheral in conjunction with the LDMA
to continuously sample two single-ended input channels, all while remaining in 
EM2. Sample results are stored in an internal array. Sample rate is 833 ksps, 
and the ADC reads GPIO pins PB00 and PB01 as inputs. With each LDMA completed 
transfer, a GPIO PC05 is toggled.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() have no
effect as oscillators are automatically turned on/off based on demand from the peripherals;
CMU_ClockEnable() is a dummy function for EFR32xG21 for library consistency/compatibility.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "scanBuffer" to the Expressions Window
4. Monitor GPIO output (see below) on the Wireless Starter Kit and watch output
   toggle with each LDMA transfer completion.
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
               are captured triggering LDMA interrupt; LDMA data transfer continues.
               			   
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