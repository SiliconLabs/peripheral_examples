switch_led_interrupt

This project demonstrates how to use GPIO pins to trigger external interrupts.
If push-button 0 is pressed, then LED0 will toggle. If push-button 1 is pressed,
then LED1 will toggle. The GPIO has two interrupt handler, and will be triggered
based on the whether the GPIO pin that triggered the interrupt is an odd or even
pin.

Note: PB0 on EFR32xG22, EFR32xG23, EFR32xG24, EFR32xG25, and EFR32xG28 is used
as an "escape hatch". This is a way to pause the device so that a debugger can
connect in order to erase flash, among other things. Before proceeding with this
example, make sure PB0 is not pressed.

How To Test:
1. Build the project and download to the Starter Kit
2. Press PB0 to toggle LED0
3. Press PB1 to toggle LED1

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz
EMU

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PD02 -  LED0
PD03 -  LED1
PB00 -  Push Button PB0
PB01 -  Push Button PB1

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 -  LED0
PD03 -  LED1
PB01 -  Push Button PB0
PB03 -  Push Button PB1

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  Push Button PB0
PB02 -  LED0
PB03 -  Push Button PB1
PB04 -  LED1

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  Push Button PB0
PB01 -  Push Button PB1
PC06 -  LED0
PC07 -  LED1

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  Push Button PB0
PB03 -  Push Button PB1
PB02 -  LED0
PD03 -  LED1