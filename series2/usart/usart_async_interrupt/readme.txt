This project demonstrates interrupt-drive operation of the USART in
asynchronous mode.  USART0 is configured for asynchronous operation at
115200 baud with 8 data bits, no parity, and one stop bit (115200N81).
The main loop waits until 80 characters or a carriage return are received
and then echos these back to the user.

================================================================================

Peripherals Used:

GPIO
USART0

The CMU is used indirectly via the USART_InitAsync() function to calculate the
divisor necessary to derive the desired baud rate.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit.
2. Connect a PC to a USB-to-serial converter, such as the Silicon Labs
   CP2102N-EK, which provides access to the CMOS level (3V) RX and TX pins.
3. Connect the RX and TX pins on the USB-to-serial converter to pins 14 and
   12, respectively on the Expansion Header of the Wireless Starter Kit
   Mainboard.
4. Open a terminal program and configure it for 115200N81 operation on the
   serial port assigned to the USB-to-serial converter.
5. Type some characters in the terminal program (they will not show) and press
   Enter to have the MCU echo them.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG21A010F1024IM32 (this code will run unchanged on any
EFR32MG21 or EFR32BG211 variant so long as the pins used as present).

PA5 - USART0_RX (Expansion Header Pin 14)
PA5 - USART0_TX (Expansion Header Pin 12)