switch_led_interrupt

This project demonstrates how to use GPIO pins to trigger external interrupts.
If push-button 0 is pressed, then LED1 will toggle. The GPIO's even interrupt
handler will be triggered when based on an even pin.

Note: PB0 on EFR32xG27 is used as an "escape hatch". This is a way to pause the
device so that a debugger can connect in order to erase flash, among other
things. Before proceeding with this example, make sure PB0 is not pressed.

How To Test:
1. Build the project and download to the Starter Kit
2. Press PB0 to toggle LED1

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz
EMU

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 - Push Button PB0
PB01 - LED1
