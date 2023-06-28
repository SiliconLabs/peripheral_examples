timer_single_edge_output_compare

This project demonstrates single output compare using the TIMER module.
TIMER0 is initialized for output compare on capture/compare channel 0,
which is routed to the GPIO pin. The compare value is set such that
after 3 seconds, the GPIO pin will be set high.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to measure the GPIO pin specified below
3. The pin should go high after 3 seconds

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