euart_async_dma_em2

This project demonstrates low-frequency operation of the EUART using LDMA
to receive inbound data and transmit outbound data while remaining in EM2.

Although this example effectively demonstrates half-duplex operation by
first receiving a set number of characters and then echoing these back,
there is nothing that prevents the LDMA from simultaneously servicing
transmit and receive data.  However, with the CPU sleeping in EM2, there
is no way to examine the inbound data stream except by otherwise waking
after receiving a set number of characters.

That being said, with the PRS and the ability of the LDMA to process
linked lists of descriptors, it would, for example, be possible to have
some other stimulus (e.g. a rising or falling edge on a designated pin)
cause a peripheral that is available in EM2, like the IADC, to produce
data and have the EUART transmit it to another device in the system, all
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

NOTE: Throughout this example, EUSART API calls are used to configure and
access EUART functionality.  This is because the EUART is a proper subset
of the EUSART in that it excludes support for synchronous (SPI) operation.

================================================================================

Peripherals Used:
LFXO
EUART0 - 9600 baud, 8-N-1 (8 data bits, no parity, one stop bit)
LDMA

The CMU is used indirectly via the EUSART_InitLf() function to calculate the
divisor necessary to derive the desired baud rate.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit.
2. Configure the JLink CDC to use 9600 baud by issuing the command
   "serial vcom config speed 9600" in the Admin console.  See this link
   for more information:
   https://community.silabs.com/s/article/wstk-virtual-com-port-baudrate-setting
3. Open a terminal program and configure it for 9600N81 operation on the
   "JLink CDC UART Port" that is provided by the board controller on the
   Starter Kit mainboard.
4. Type some characters in the terminal program (they will not show) and
   press Enter to have the MCU echo them.

Alternatively, the example may be tested with a USB-to-serial converter,
such as the Silicon Labs CP2102N-EK.  Refer to the list below for the
mapping of EUSART signals to Expansion (EXP) Header pins.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PA6 - EUART0_RX (Expansion Header Pin 14)
PA5 - EUART0_TX (Expansion Header Pin 12)
