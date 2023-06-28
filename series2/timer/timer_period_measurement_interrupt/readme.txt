timer_period_measurement_interrupt

This project demonstrates period measurement using TIMER0.  The module
is configured to request an interrupt after capturing a pair of falling
edges.  In the IRQ handler, in addition to saving the two edge times,
the overflow flag is checked in order to account for two edges that span
the time during which the counter rolls over from 0xFFFFFFFF (TIMER0 is
32 bits wide) to 0.

Upon exiting the IRQ handler, the period is calculated and returned as
an integer value in microseconds, thus the measuredPeriod value will
show 1000 for an input signal with a period of 1 kHz.

Note: The range of frequencies this program can measure accurately is
limited by the selected frequency of the EM01GRPACLK, prescaling of the
local TIMER clock, and the counter width of the selected TIMER, e.g.
TIMER0 is 32 bits wide while other TIMERs are generally 16 bits wide.

Note: On EFR32xG21 devices, calls to CMU_ClockEnable() have no effect
as clocks are automatically turned on/off in response to on-demand
requests from the peripherals.  CMU_ClockEnable() is a dummy function
on EFR32xG21 present for software compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Connect a periodic signal to the GPIO pin specified below
3. Go into debug mode and click run
4. View the measuredPeriod (time period in us) global variable in the debugger

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