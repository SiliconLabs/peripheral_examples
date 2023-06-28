timer_pwm_dma

This project demonstrates DMA-driven pulse width modulation using the
TIMER module.  TIMER0 is initialized for PWM on capture/compare channel 0
and routed to a GPIO pin.

In PWM mode, compare events set the output pin and overflow events clear
it such that the values in the TIMER_CC_OC and TIMER_TOP registers
specify the PWM duty cycle and frequency, respectively.

Each time a compare event occurs, the LDMA responds by writing a new
value to the TIMER_CC_OCB register such that, in this example, the
output duty cycle increases from 0 to 100% by an increment of 10%,
then rolls over back to 0% and repeats.

Note: On EFR32xG21 devices, calls to CMU_ClockEnable() have no effect
as clocks are automatically turned on/off in response to on-demand
requests from the peripherals.  CMU_ClockEnable() is a dummy function
on EFR32xG21 present for software compatibility.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit.
2. Use an oscilloscope to view the 1 kHz signal with continuosly varying
   duty cycle on the expansion header (EXP) pin specified below.

================================================================================

Peripherals Used:
CMU    - HFRCO @ 19 MHz
TIMER0 - EM01GRPACLK
LDMA

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