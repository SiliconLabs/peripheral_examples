iadc_single_low_current

This project demonstrates one of the lowest current consumption configurations
for the IADC peripheral. The IADC is configured to utilize the internal IADC timer
to trigger a single single-ended conversion. Upon completion, an LDMA data transfer 
occurs storing the raw data to memory. Both the IADC and LDMA run in EM2, and this
project goes a step further to shutdown onboard SPI FLASH on the WSTK, along with 
changing the system clock from FSRCO. The RTCC peripheral is also included in this 
example and run from the LFRCO. Typical current consumption while retaining RAM is
documented in the Series 2 device datasheet. This approximate current value does not
include additional current for the clock trees and the IADC. This project is intended
to be profiled with Simplicity Studio's Energy Profiler to observe the current consumption
while in EM2.

How To Test:
1.  Update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  Open Simplicity Studio and right click on the main project directory in Project Explorer
4.  In the drop-down menu, select "Profile As Simplicity Energy Profiler Target"
5.  The project will compile and load and start-up in EM0 awaiting user input to proceed
6.  Observe the current consumption in Energy Profiler of the kit in EM0
6.  Press and release PB0 on the WSTK to initiate the IADC and LDMA and enter EM2
7.  Observe in Energy Profiler the drop in current consumption while in EM2; for this 
    example's configuration (100 samples/second) the EM2 current should drop
    EFR32xG21 ~45uA; EFR32xG22 ~23uA
    *There is some part-to-part/board-to-board variance; this is a rough typical current*	
8.  After completing NUM_SAMPLES conversions, the LDMA will wake the MCU from EM2 and 
    turn on LED0 to indicate transfer completion
9.  Again, observe in Energy Profiler the rise in current consumption when returning to EM0,
    now with the additional current sourcing the LED
10. Energy Profiler can at this point be paused and the data saved. UG343: Multi-Node Energy
    Profiler User's Guide and Simplicity Studio's built-in help menu contain additional
    information to help analyze power consumption and profile energy usage.	

Users can experiment with various IADC clock frequencies and timer cycles to determine
optimum settings for desired sampling rates.

Peripherals Used:
CLK_CMU_ADC  - 1 MHz HFRCOEM23 for EFR32xG21;  20 MHz FSRCO for EFR32xG22
CLK_SRC_ADC  - 1 MHz for EFR32xG21;            5 MHz for EFR32xG22
CLK_ADC      - 1 MHz for EFR32xG21;            1 MHz for EFR32xG22
IADC         - 12-bit resolution, Automatic Two's Complement (single-ended = unipolar) 
               unbuffered 3.3V (AVDD) IADC voltage reference
               IADC and reference kept in normal mode, requiring a warm-up period prior
			   to each conversion
			 - Conversions triggered by IADC timer
LDMA         - 32-bit transfer from IADC to buffer, loop continuously until NUM_SAMPLES
               are captured triggering LDMA interrupt which halts IADC scan
RTCC         - 32768 Hz LFRCO clock source, 256 prescaler 
               
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PD03 -  IADC input, single-ended, Expansion Header (see source for specific
pin)
PD02 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PD02 -  GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PC04 -  IADC input, single-ended, J102 of BRD4001, Pin 25
PB00 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK