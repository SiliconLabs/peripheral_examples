eusart_async_dma_em2

This project uses the EUSART to receive serial input and then echo it back using
the LDMA. The devices stays in EM2 the entire time since the LDMA module can 
still operate in EM2.

When EFR32xG22 and later devices enter EM2/3, the clock to the CPU debug
block is stopped, effectively dropping the host OC debugger connection.
This examples includes an "escape hatch" mechanism to pause the device so
that a debugger can connect in order to erase flash.  To use this, press
the PB0 push button and, while holding the button, press the RESET button
in the lower right corner of the WSTK.  Upon releasing the RESET button,
the device runs code that sees that PB0 is depressed, turns on LED0, and
issues a breakpoint instruction to halt the CPU.  At this point, a
the debugger connection can be resumed to erase flash, etc.

================================================================================

Peripherals Used:
LFXO
EUSART0 - 9600 baud, 8-N-1 (8 data bits, no parity, one stop bit)
LDMA

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit.
2. Use a USB to serial device (such as the CP210x). Connect the CP210x's RX pin
   to the device's TX pin. Connect the CP210x's TX pin to the device's RX pin.
   These port and pin mappings are listed below.
3. Open up a serial terminal device such as Termite.
4. In Termite, open the port connected to the CP210x device (check which port
   using Device Manager).
5. Input will be echoed back, after typing in Termite.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32FG23 868-915 MHz 14 dBm Radio Board (BRD4263B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG23A010F512GM48
PA8 - USART0_TX (Expansion Header Pin 12)
PA9 - USART0_RX (Expansion Header Pin 14)
