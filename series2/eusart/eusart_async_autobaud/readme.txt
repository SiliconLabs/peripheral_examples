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
2. Open a terminal program and configure it for 8-N-1 operation with desired 
   baud rate(eg: 115200) on the "JLink CDC UART Port" that is provided by 
   the board controller on the Starter Kit mainboard.
3. Set a breakpoint at the end of the EUSART1_RX_IRQHandler and run the example 
   project.
4. Observe EUSART1_CLKDIV value after EUSART1 initalization and type character 
   "U" in the terminal program.
5. At the breakpoint, observe EUSART1_CLKDIV getting updated acoording to the 
   desired baudrate selected in the terminal program and remove the breakpoint.
5. Type some characters in the terminal program (they will not show) and
   press Enter to have the MCU echo them.

Note: 
By default VCOM of WSTK board controller is factory programmed to support 
115200bps. To use other baud rates, follow the instructions below:
1. In Simplicity Studio Launcher tab, select the required J-Link adapter in the
   Debug Adapters window, right-click on it and click on the Launch Console.
2. When the console appears Switch to the Admin tab at the top, then click 
   into the input text box at the bottom and press Enter. Now the WSTK> prompt 
   appears.
3. To change the Virtual COM port baud rate to the desired baud rate in bps 
   issue the following command in Admin tab of Launch Console:
   serial vcom config speed <desired baud rate in bps>

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
