eusart_async_dma_em2

This project demonstrates low-frequency operation of EUSART0 using LDMA to
receive inbound data and transmit outbound data while remaining in EM2.
Note that EUSART0 is used because it supports operation in EM2 from a
low-frequency operation.  Other EUSARTs are limited to operation in EM0/1
only.  See the configuration summary table in the specific device datasheet
for details.

Although this example effectively demonstrates half-duplex operation by
first receiving a set number of characters and then echoing these back,
there is nothing that prevents the LDMA from simultaneously servicing
transmit and receive data.  However, with the CPU sleeping in EM2, there
is no way to examine the inbound data stream except by otherwise waking
after receiving a set number of characters.

That being said, with the PRS and the ability of the LDMA to process
linked lists of descriptors, it would, for example, be possible to have
some other stimulus (e.g. a rising or falling edge on a designated pin)
cause a peripheral that is available in EM2, like the IADC, produce data
and have the EUSART transmit it to another device in the system, all
without the need to wake the CPU and operate in EM0/1.

When EFR32xG22 and later devices enter EM2/3, the clock to the CPU debug
logic is stopped, effectively dropping the host debugger connection.
This example includes an "escape hatch" mechanism to pause the device so
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

The CMU is used indirectly via the EUSART_InitLf() function to calculate the
divisor necessary to derive the desired baud rate.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit.
2. Open a terminal program and configure it for 9600N81 operation on the
   "JLink CDC UART Port" that is provided by the board controller on the
   Starter Kit mainboard.
3. Type some characters in the terminal program (they will not show) and
   press Enter to have the MCU echo them.

Alternatively, the example may be tested with a USB-to-serial converter,
such as the Silicon Labs CP2102N-EK.  Refer to the list below for the
mapping of EUSART signals to Expansion (EXP) Header pins.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32FG23 868-915 MHz 14 dBm Radio Board (BRD4263B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG23A010F512GM48
PA8 - USART0_TX (Expansion Header Pin 12)
PA9 - USART0_RX (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24A010F1536GM48 (this code will run unchanged on any
        EFR32MG24 variant so long as the pins used as present).
PA8 - EUSART0_TX (Expansion Header Pin 12)
PA9 - EUSART0_RX (Expansion Header Pin 14)
PB0 - VCOM_ENABLE (WSTK P15)