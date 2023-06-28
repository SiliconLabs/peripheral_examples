timer_pulse_capture

This project demonstrates single pulse capture using the TIMER module.
The HFXO is configured and selected as the EM01GRPACLK source, which is
the clock branch for high-frequency TIMERs on all Series 2 EFM32/EFR32
devices.  The TIMER is then configured for input capture on channel CC0
for all edges but with interrupts on every other edge in order to reduce
the CPU overhead.  The TIMER input capture logic is double-buffered and
can capture two subsequent edges before requesting an interrupt.

The GPIO pin specified below must be connected to a periodic signal or
pulse generator.  The two edges captured (one falling and one rising) are
read from the CCV register.

Note: The range of frequencies this program can measure accurately is
limited by the selected frequency of the EM01GRPACLK, prescaling of the
local TIMER clock, and the counter width of the selected TIMER, e.g.
TIMER0 is 32 bits wide while other TIMERs are generally 16 bits wide.
The minimum measurable period is around 700 ns (about 1.43 MHz).

Note: On EFR32xG21 devices, calls to CMU_ClockEnable() have no effect
as clocks are automatically turned on/off in response to on-demand
requests from the peripherals.  CMU_ClockEnable() is a dummy function
on EFR32xG21 present for software compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Connect a periodic signal to GPIO pin specified below
3. Go into debug mode and click run
4. View the firstEdge and secondEdge global variables in the watch window

================================================================================

Peripherals Used:
TIMER - CC0

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
CMU   - HFXO @ 38.4 MHz
PA06  - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A)
        + Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
CMU   - HFXO @ 38.4 MHz
PA06  - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
CMU   - HFXO @ 39 MHz
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
CMU   - HFXO @ 39 MHz
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
CMU   - HFXO @ 39 MHz
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32MG27 Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
CMU   - HFXO @ 39 MHz
PA06  - TIM0_CC0 (WSTK P11, Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C)
        + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
CMU   - HFXO @ 39 MHz
PB04  - TIM0_CC0 (WSTK P8, Expansion Header Pin 11)