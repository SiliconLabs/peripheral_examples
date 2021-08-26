eusart_async_half_duplex

This example uses the EUSART0 module to do half-duplex communication with the 
EUSART0 module on another board using the single data-link configuration 
described in the reference manual. This means that half-duplex is done by 
using only one wire to communicate between EUSARTs. In this example, we
ensure that only one EUSART is transmitting at a time by starting one EUSART
with transmitter enabled and receiver disabled, while the other EUSART has its
receiver enabled and transmitter disabled. Each EUSART switches between
transmit/receive mode when it is done transmitting/receiving (i.e. it
transmitted or received a '\n' character).

Note: you MUST change which EUSART starts transmitting/receiving first by
defining ONLY ONE of the INITIAL_TRANSMITTER and INITIAL_RECEIVER macros at the
top of the source file. One board should have INITIAL_TRANSMITTER defined and
the other board should have INITIAL_RECEIVER defined. By default,
INITIAL_TRANSMITTER is defined.

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

GPIO
EUSART0 - 9600 baud, 8-N-1 (8 data bits, no parity, one stop bit)

The CMU is used indirectly via the EUSART_InitHf() function to calculate the
divisor necessary to derive the desired baud rate.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit.
2. Choose one board to be the initial transmitter and make sure the
   INITIAL_TRANSMITTER macro is defined at the top of the source file.
3. Choose the other board to be the initial receiver and make sure the
   INITIAL_RECEIVER macro is defined at the top of the source file.
4. Connect the two boards' EUSART TX pins together
5. Connect EUSART TX pin of one of the board to RX pin of the serial-to-USB device
   (such as CP210x) 
6. Open up a serial terminal device such as Termite.
7. In Termite, open the port connected to the CP210x device.
8. If successful, Termite will show the following:
   Initial RX: Receive success and transmitting now
   Initial TX: Receive success and transmitting now
   Initial RX: Receive success and transmitting now
   Initial TX: Receive success and transmitting now
   Initial RX: Receive success and transmitting now
   Initial TX: Receive success and transmitting now
9. Note: you might have to reset the receiver board first and then the
   transmitter board afterwards to see the correct output. The reason for this
   is because the transmitter board could send the data and switch to receiver
   mode before the receiver board is set up. The easiest way this could happen
   is if you flashed the transmitter board first and then the receiver board and
   then connected the two TX pins. This would result in both boards
   being stuck in receiver mode.
10. You could also see the output by using the debugger

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32FG23 868-915 MHz 14 dBm Radio Board (BRD4263B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG23A010F512GM48
PA8 - USART0_TX (Expansion Header Pin 12)