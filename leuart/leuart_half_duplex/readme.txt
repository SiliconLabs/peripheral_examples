
================================================================================

This example uses the LEUART0 module (low energy UART) to do half-duplex
communication with the LEUART0 module on another board using the single data-link
configuration described in the reference manual. This means that half-duplex is
done by using only one wire to communicate between LEUARTs. In this example, we
ensure that only one LEUART is transmitting at a time by starting one LEUART
with transmitter enabled and receiver disabled, while the other LEUART has its
receiver enabled and transmitter disabled. Each LEUART switches between
transmit/receive mode when it is done transmitting/receiving (i.e. it
transmitted or received a '\n' character).

Note: you MUST change which LEUART starts transmitting/receiving first by
defining ONLY ONE of the INITIAL_TRANSMITTER and INITIAL_RECEIVER macros at the
top of the source file. One board should have INITIAL_TRANSMITTER defined and
the other board should have INITIAL_RECEIVER defined. By default,
INITIAL_TRANSMITTER is defined.

The board waits in EM2 to preserve energy while waiting for input. The GG11 
board in this example used about 102 microamps on average. The PG12 board used 
about 69 microamps. After commenting out the line of code that puts the board in 
EM2, the GG11 board used about 2.1 mA and the PG12 board used about 1.6 mA.
Note: this energy measurement was done using Simplicity Studio's built-in energy
profiler for communication between a GG11 board and a PG12 board with a Debug
build configuration and no optimization flags (gcc -O0).

================================================================================

There is a lot of code in this example, but the important things to take away
when importing this example into your own code are the following:

1. initGpio() - Shows how to initialize the GPIO pins used for LEUART
                communication.

2. initLeuart() - Shows which clocks to use for the LEUART modules and how to
                  initialize them. Also shows how this example uses the loopback
                  and autotristate features. Loopback mode connects the receiver
                  to the TX pin (allowing you to both transmit and receive on
                  the TX pin). Autotristate automatically tristates the
                  transmitter when it is idle (this is important because you
                  don't want to inadvertently send data when you are in receive
                  mode).

3. switchToTxMode() - Shows how to disable the receiver and enable the
                      transmitter. If you don't disable the receiver when you
                      start transmitting, you will get an RX interrupt (along
                      with the TX interrupt) every time you try to send data
                      because loopback mode connects the receiver to the
                      transmitter. To prevent this, you can call this function
                      before you start transmitting.

4. switchToRxMode() - Similar to switchToTxMode().

5. LEUART0_IRQHandler() - When debugging your code, it is important to make sure
                          that only RX portion or TX portion of the interrupt
                          handler gets executed since only one LEUART should be
                          transmitting at a time to avoid data collisions. If
                          the RX and TX portion are both being executed, then it
                          means the transmitter/receiver aren't being properly
                          disabled/enabled. It is also important to see that
                          after one LEUART finishes transmitting its message, it
                          calls switchToRxMode to disable its transmitter and
                          enable its receiver.

================================================================================

For further help, see the Half Duplex Communication section in the LEUART
chapter of the Reference Manual.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit.
2. Choose one board to be the initial transmitter and make sure the
   INITIAL_TRANSMITTER macro is defined at the top of the source file.
3. Choose the other board to be the initial receiver and make sure the
   INITIAL_RECEIVER macro is defined at the top of the source file.
4. Connect the two boards' LEUART TX pins together
5. Connect the RX pin of the serial to USB device (such as the CP210x) to either
   of the TX pins. Note: since it is a little difficult to connect all three
   pins (TX of one board, TX of another board, RX of the CP210x device) together
   to the same node, it might help to use a breadboard.
6. Open up a serial terminal device such as Termite.
7. In Termite, open the port connected to the CP210x device (check which port
   using Device Manager).
8. If successful, Termite will show the following:
   LEUART half duplex code example
   Initial Receiver: Receive success and transmitting now
   Initial Transmitter: Receive success and transmitting now
   Initial Receiver: Receive success and transmitting now
   Initial Transmitter: Receive success and transmitting now
   Initial Receiver: Receive success and transmitting now
   Initial Transmitter: Receive success and transmitting now
9. Note: you might have to reset the receiver board first and then the
   transmitter board afterwards to see the correct output. The reason for this
   is because the transmitter board could send the data and switch to receiver
   mode before the receiver board is set up. The easiest way this could happen
   is if you flashed the transmitter board first and then the receiver board and
   then connected the two TX pins. This would result in both boards
   being stuck in receiver mode.
10. You could also see the output by using the debugger

================================================================================

Peripherals Used:
LFXO - 32.768 kHz (reference clock for the LFB clock branch)
LEUART0 - 9600 baud, 8-N-1 (8 data bits, no parity, one stop bit)

================================================================================

Listed below are the port and pin mappings for working with this example. Unless
explicity specified otherwise, the pin corresponds to the pin on the expansion
headers.

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD4 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD4 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD4 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD4 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD4 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD4 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PD4 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PD10 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A)
Device: EFM32MG1P232F256GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PA0 - LEUART0 TX (top middle row of breakout pads, Pin 33)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A)
Device: EFM32FG1P133F256GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PA0 - LEUART0 TX (top middle row of breakout pads, Pin 33)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A)
Device: EFM32BG1P232F256GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PA0 - LEUART0 TX (top middle row of breakout pads, Pin 33)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PA0 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC14 - LEUART0 TX (Pin 12)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC14 - LEUART0 TX (top right row of breakout pads) (Pin 25)

