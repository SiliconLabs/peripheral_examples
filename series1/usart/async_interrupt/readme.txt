async_interrupt

This project demonstrates interrupt-drive operation of the USART in
asynchronous mode.  Depending on the device, USART0, USART1, or USART4 is
configured for asynchronous operation at 115200 baud with 8 data bits, no
parity, and one stop bit (115200N81).  The main loop waits until 80
characters or a carriage return are received and then echos these back to
the user.

================================================================================

Peripherals Used:

CMU
EMU
GPIO
USART0 (xG1/xG12/xG13/xG14)
USART1 (TG11)
USART4 (GG11)

Apart from enaling module clocks, the CMU is used indirectly via the
USART_InitAsync() function to calculate the divisor necessary to derive
the desired baud rate.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit.
2. Open a terminal program and configure it to use the JLink CDC UART Port
   that is assigned to the (Wireless) Starter Kit virtual COM port.  This
   can vary from board to board and PC to PC.  Check the appropriate
   operating system tool (e.g. the Windows Device Manager) to determine the
   port assignment.
3. Set the frame parameters to 115200 baud, no parity, 8 data bits, and
   1 stop bit on the serial port assigned to the virtual COM port.
5. Type some characters in the terminal program (they will not show) and press
   Enter to have the MCU echo them.

================================================================================

This example has the following targets:

Board: Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125

Board: Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG1P232F256GM48

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG12P332F1024GL125

Board: Silicon Labs EFR32BG13 Starter Kit (BRD4104A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG1P133F256GM48

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG12P433F1024GL125

Board: Silicon Labs EFR32FG13 Starter Kit (BRD4256A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48

Board: Silicon Labs EFR32FG14 Starter Kit(BRD4257A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFM32MG1P232F256GM48

Board:  Silicon Labs EFR32MG12 Radio Board (BRD4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125

Board: Silicon Labs EFR32MG13 Starter Kit (BRD4159A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48

Board: Silicon Labs EFR32MG14 Starter Kit (BRD4169B) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
