eusart_async_polled

This project demonstrates polled operation of the EUSART in asynchronous
mode.  EUSART1 is configured for asynchronous operation at 115200 baud
with 8 data bits, no parity, and one stop bit (115200N81).  The main loop
waits until 80 characters or a carriage return are received and then echos
these back to the user.

================================================================================

Peripherals Used:

GPIO
EUSART1

The CMU is used indirectly via the EUSART_UartInitHf() function to calculate
the divisor necessary to derive the desired baud rate.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit.
2. Open a terminal program and configure it for 115200N81 operation on the
   "JLink CDC UART Port" that is provided by the board controller on the
   Starter Kit mainboard.
3. Type some characters in the terminal program (they will not show) and
   press Enter to have the MCU echo them.

Alternatively, the example may be tested with a USB-to-serial converter,
such as the Silicon Labs CP2102N-EK.  Refer to the list below for the
mapping of EUSART signals to Expansion (EXP) Header pins.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48 (this code will run unchanged on any
        EFR32ZG23 variant so long as the pins used as present).
PA8 - EUSART1_TX (Expansion Header Pin 12)
PA9 - EUSART1_RX (Expansion Header Pin 14)
PB0 - VCOM_ENABLE (WSTK P15)

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48 (this code will run unchanged on any
        EFR32MG24 variant so long as the pins used as present).
PA8 - EUSART1_TX (Expansion Header Pin 12)
PA9 - EUSART1_RX (Expansion Header Pin 14)
PB0 - VCOM_ENABLE (WSTK P15)

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56 (this code will run unchanged on any
        EFR32FG25 variant so long as the pins used as present).
PA8 - EUSART1_TX (Expansion Header Pin 12)
PA9 - EUSART1_RX (Expansion Header Pin 14)
PA11 - VCOM_ENABLE (WSTK P15)