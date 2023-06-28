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
3. Type 10 characters (will not show if it is less than 10) in the terminal
   program and watch the characters echo back from the terminal.

NOTE: The WSTK board controller defaults to 115200 baud for the virtual
COM port (JLink CDC UART Port).  Follow these steps to switch to 9600
baud:
1. From the Simplicity Studio Launcher, select the J-Link adapter in the
   Debug Adapters window, right-click on it and select Launch Console.
2. When the console appears, switch to the Admin tab, click in the input
   area, and press Enter.  The WSTK> prompt will appear.
3. To change the virtual COM (VCOM) port baud rate, enter the following
   command:

   serial vcom config speed 9600

Alternatively, the example may be tested with a USB-to-serial converter,
such as the Silicon Labs CP2102N-EK.  Refer to the list below for the
mapping of USART signals to Expansion (EXP) Header pins.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PA08 -  EUSART0_TX (Expansion Header Pin 12)
PA09 -  EUSART0_RX (Expansion Header Pin 14)
PB00 -  VCOM_ENABLE (WSTK P15)

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48 (this code will run unchanged on any
        EFR32MG24 variant so long as the pins used as present).
PA08 -  EUSART0_TX (Expansion Header Pin 12)
PA09 -  EUSART0_RX (Expansion Header Pin 14)
PB00 -  VCOM_ENABLE (WSTK P15)

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56 (this code will run unchanged on any
        EFR32FG25 variant so long as the pins used as present).
PA08 -  EUSART0_TX (Expansion Header Pin 12)
PA09 -  EUSART0_RX (Expansion Header Pin 14)
PA11 -  VCOM_ENABLE (WSTK P15)

Board:  Silicon Labs EFR32xG27 Buck Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PA05 -  EUSART0_TX (Expansion Header Pin 12)
PA06 -  EUSART0_RX (Expansion Header Pin 14)
PB04 -  VCOM_ENABLE (WSTK_P15)

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PA08 -  EUSART0_TX (WSTK Pin 28)
PA09 -  EUSART0_RX (WSTK Pin 30)
PB00 -  VCOM_ENABLE (WSTK_P15)
