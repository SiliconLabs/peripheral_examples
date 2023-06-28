timer_frequency_generation

This project demonstrates frequency generation using the TIMER module.
TIMER0 is initialized for output compare on Compare/Capture channel 0 which
is routed to a GPIO Pin. The Top value is set such that on each overflow, 
the output toggles at the desired frequency: OUT_FREQ initially set to 1000 Hz.

NOTE: The range of frequencies the program can generate is limited. The maximal
frequency is 1/2 the clock frequency (Setting the top value to 0 causes an 
overflow on each clock cycle). The minimum frequency is
(clock frequency) / (2 * 65535 * prescale). By default this minimum frequency is 
(19 * 10^6) / (2 *65535 * 1) = 145Hz.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Measure waveform on GPIO pin (see board specific pin below)

================================================================================

Peripherals Used:
CMU    - HFRCO @ 19 MHz
TIMER0 - EM01GRPACLK

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA06  - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A)
        + Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA06  - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32MG27 Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
PA06  - TIM0_CC0 (WSTK P11, Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C)
        + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB04  - TIM0_CC0 (WSTK P8, Expansion Header Pin 11)