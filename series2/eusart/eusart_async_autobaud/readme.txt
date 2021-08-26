eusart_async_autobaud

This project demonstrates Auto Baud Detection feature in interrupt-drive 
operation of the EUSART in asynchronous mode.  EUSART1 is configured for 
asynchronous operation with autobaud, 8 data bits, no parity, and one stop bit.

The main loop waits for sync word (0x55 or ASCII character 'U') on the RX pin to
set the baud rate and then it waits until 80 characters or a carriage return are
received to echo these back to the user.

================================================================================

Peripherals Used:

GPIO
EUSART1

================================================================================

How To Test:
1. Build the project and download to the Starter Kit.
2. Open a terminal program and configure it for N81 operation with desired 
   baud rate(eg: 115200 & 9600) on the "JLink CDC UART Port" that is provided by 
   the board controller on the Starter Kit mainboard.
3. Set a breakpoint at the end of the EUSART1_RX_IRQHandler and run the example 
   project.
4. Observe EUSART1_CLKDIV value after EUSART1 initalization and type character 
   "U" in the terminal program.
5. At the breakpoint, observe EUSART1_CLKDIV getting updated acoording to the 
   desired baudrate selected in the terminal program and remove the breakpoint.
5. Type some characters in the terminal program (they will not show) and
   press Enter to have the MCU echo them.

Alternatively, the example may be tested with a USB-to-serial converter,
such as the Silicon Labs CP2102N-EK.  Refer to the list below for the
mapping of USART signals to Expansion (EXP) Header pins.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32FG23 868-915 MHz 14 dBm Radio Board (BRD4263B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG23A010F512GM48
PA8 - USART0_TX (Expansion Header Pin 12)
PA9 - USART0_RX (Expansion Header Pin 14)
