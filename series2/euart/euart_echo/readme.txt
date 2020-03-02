euart_echo

This project demonstrates interrupt-drive operation of the EUART.  EUART0 is 
configured for asynchronous operation at 115200 baud with 8 data bits,  no 
parity, and one stop bit (115200N81). The main loop waits until 80 characters 
or a carriage return are received and then echos these back to the user.

================================================================================
Peripherals Used:

GPIO
EUART0 - 115200 baud, 8-N-1 (8 data bits, no parity, one stop bit)

The CMU is used indirectly via the EUSART_InitHf() function to calculate the
divisor necessary to derive the desired baud rate.

================================================================================
How To Test:
1. Build the project and download to the Starter Kit.
2. Connect a PC to a USB-to-serial converter, such as the Silicon Labs
   CP2102N-EK, which provides access to the CMOS level (3V) RX and TX pins.
3. Connect the USB-to-serial converter's RX pin to the device's TX pin and 
   connect USB-to-serial converter's TX pin to the device's RX pin. 
   These port and pin mappings are listed below.
4. Open a terminal program and configure it for 115200N81 operation on the
   serial port assigned to the USB-to-serial converter.
5. Type some characters in the terminal program (characters may/may not display 
   depending on terminal settings) and press enter to have the MCU echo them.

================================================================================
Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PC1 - EUART0_RX (Expansion Header Pin 6)
PC0 - EUART0_TX (Expansion Header Pin 4)