iadc_single_low_current

This project demonstrates one of the lowest current consumption configurations
for the IADC peripheral. The IADC is configured to utilize the internal IADC 
timer to trigger a single single-ended conversion. Upon completion, an LDMA data
transfer occurs storing the raw data to memory. Both the IADC and LDMA run in 
EM2, and IADC is configured to run in normal mode (shuts down IADC between 
conversions) and wake LDMA for transfer with data valid level of 4 (FIFO full) 
to conserve energy. Additionally, if onboard SPI FLASH is present on the radio 
board, this is powered down, along with any used but no longer required clock 
trees. The RTCC/BURTC peripheral is also included in this example and run from 
the LFRCO. Typical current consumption while retaining RAM is documented in the 
Series 2 device datasheet. This approximate current value does not include 
additional current for the clock trees and the IADC. This project is intended 
to be profiled with Simplicity Studio's Energy Profiler to observe the current 
consumption while in EM2.

Note: To debug EFR32xG23, the DEBUG define must be set to enable debug 
connectivity in EM2; this is cleared by default for lowest energy while 
observing using Energy Profiler.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1.  Update the kit's firmware from the Simplicity Launcher (if necessary)
2.  Build the project and download to the Starter Kit
3.  Open Simplicity Studio and right click on the main project directory in 
    Project Explorer
4.  In the drop-down menu, select "Profile As Simplicity Energy Profiler Target"
5.  The project will compile and load and start-up in EM0 awaiting user input to
    proceed
6.  Observe the current consumption in Energy Profiler of the kit in EM0
7.  Press and release PB0 on the WSTK to initiate the IADC and LDMA and enter 
    EM2
8.  Observe in Energy Profiler the drop in current consumption as the MCU 
    transitions from EM0 to EM2; for this example's configuration (100 
    samples/second) the approximate EM2 current consumption measurements for 
    each devices are as follows:
    EFR32xG21 ~45uA
    EFR32xG22 ~23uA
    EFR32xG23 ~23uA
    EFR32xG24 ~23uA
    EFR32xG25 ~34uA
    EFR32xG27 ~25uA
    EFR32xG28 ~28uA
    *There is some part-to-part/board-to-board variance; this is a rough typical
    current*	

9.  After completing NUM_SAMPLES conversions, the LDMA will wake the MCU from 
    EM2 and toggle LED to indicate transfer completion before returning to EM2 
    until the next transfer completes; additional current draw in profiler is 
    LED current consumption
10. Energy Profiler can at this point be paused and the data saved. 
    UG343: Multi-Node Energy Profiler User's Guide and Simplicity Studio's 
    built-in help menu contain additional information to help analyze power 
    consumption and profile energy usage.	

Note for Testing on xG28:
On BRD4400C, there is a diversity SPDT switch on the 2.4 GHz radio output that
will consume ~63 uA of additional current when supplied power via logic high on
GPIO PD02. The pin is configured in disabled mode (high-Z) by default, thus SPDT
switch is powered down by default. Be advised that when PD02 is driven to logic
high, observed current consumption will be higher than what is specified in the 
EFR32ZG28 device datasheet due to this additional integrated circuit.

Users can experiment with various IADC clock frequencies and timer cycles to 
determine optimum settings for desired sampling rates.

================================================================================

Peripherals Used:
CMU      - FSRCO @ 20 MHz, LFRCO @ 32.768 kHz
EMU
GPIO
IADC     - 12-bit resolution, 
         - Automatic Two's Complement (single-ended = unipolar) 
         - Internal VBGR reference with 0.5x analog gain (1.21V / 0.5 = 2.42V)
         - IADC and reference kept in normal mode, requiring a warm-up period 
           prior to each conversion
         - Conversions triggered by IADC timer
LDMA     - 32-bit transfer from IADC to buffer, loop continuously until 
           NUM_SAMPLES are captured triggering LDMA interrupt which toggle LED; 
           IADC scan runs continuously
                
Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
RTCC  - 32768 Hz LFRCO clock source, 256 prescaler
PB00  - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PA05  - IADC input, single-ended, Expansion Header Pin 12, WSTK P9
PD02  - GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) +
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
RTCC  - 32768 Hz LFRCO clock source, 256 prescaler
PD02  - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PA05  - IADC input, single-ended, Expansion Header Pin 12, WSTK P9
PB00  - GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
BURTC - 32768 Hz LFRCO clock source, 256 prescaler 
PB02  - GPIO Push/Pull output, WSTK Pin 19, LED0
PA05  - IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB01  - GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
BURTC - 32768 Hz LFRCO clock source, 256 prescaler
PB02  - GPIO Push/Pull output, WSTK Pin 19, LED0
PA05  - IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB01  - GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
BURTC - 32768 Hz LFRCO clock source, 256 prescaler
PC06  - GPIO Push/Pull output, WSTK Pin 27, LED0
PA05  - IADC input, single-ended, Expansion Header pin 7, WSTK P4
PB00  - GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
BURTC - 32768 Hz LFRCO clock source, 256 prescaler
PB01  - GPIO Push/Pull output, WSTK Pin 6, LED1
PA05  - IADC input, single-ended, Expansion Header pin 12, WSTK P9
PB00  - GPIO pull filtered input, Push Button 0 (PB0) on the WSTK

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
BURTC - 32768 Hz LFRCO clock source, 256 prescaler
PB02 -  GPIO Push/Pull output, WSTK P19, LED0
PA05 -  IADC input, single-ended, WSTK P43
PB01 -  GPIO pull filtered input, Push Button 0 (PB0) on the WSTK