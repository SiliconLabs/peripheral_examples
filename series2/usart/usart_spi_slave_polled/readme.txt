This project demonstrates polled operation of the USART in synchronous
slave mode.  USART0 is configured for SPI-compatible operation at 1 Mbps.
Compatibility with the Synchronous Peripheral Interface standard implies
a word size of 8 data bits transmitted and received MSB-first.

It is important to note that because this example runs in slave mode, the
GPIO configuration of the USART receive and transmit pins is reversed.
This is reflected in their SPI signal names: Master-In/Slave Out
(MISO) and Master-Out/Slave-In (MOSI).  So, while the USART_TX and
USART_RX pins are output and input, respectively, in master mode, they
operate as input (MOSI) and output (MISO) in slave mode.

Likewise, the clock and chip select (slave select) pins are also slave
mode inputs.  To simplify polling for the start of a transfer from the
master, the chip select input is configured to detect a falling edge.
The master device must add setup time after chip select assertion in
order for the slave device to pre-load the transmit buffer if it is
expected to shift out a byte of data concurrent with the first incoming
byte fromt the master (even if this is otherwise ignored).  The provided polled and interrupt-driven SPI master mode examples do this simply by
manually both asserting the chip select and loading the first byte of
data to be transmitted.  The DMA-driven master example includes setup
time insertion through use of the auto chip select assertion mechanism.

A single output buffer is populated with 10 bytes (set by BUFLEN) of
outbound data that will be overwritten by the incoming data.  The device
polls for the falling edge of the chip select and then clears the
receive FIFO.  This step is not necessary in a well-behaved SPI system
configuration, but because this example is intended to run with one of the master mode examples, which constantly transmit data, it is required so
that data is received in the expected order when the master asserts the
chip select.

The specified number of data bytes are transmitted and received in a
for-loop.  The emlib USART_Tx() function checks for space in the transmit
FIFO space (USART_STATUS_TXBL != 0) before loading it with each byte.
USART_Rx() checks for valid receive data (USART_STATUS_RXDATAV == 1)
before reading a byte from the FIFO.  This provides the desired polling
mechanism, which is predicated on the master driving eight clock edges
for each byte transferred to and from the slave.

================================================================================

Peripherals Used:

GPIO
USART0

The CMU is used indirectly via the USART_InitSync() function to calculate
the divisor necessary to derive the desired bit rate.

================================================================================

Test Procedure:

1. This example requires two Wireless Starter Kits (WSTKs).  Build one of
   the SPI master mode examples and download it to the first board.

2. Build this project and download it to the second board.

3. Connect the Expansion Header pins from the master to the slave board
   as follows:

     Master WSTK         Slave WSTK
    -------------       -------------
    Pin 1  (GND)  <---> Pin 1 (GND)
    Pin 14 (MOSI) <---> Pin 14 (MOSI)
    Pin 12 (MISO) <---> Pin 12 (MISO)
    Pin 10 (SCLK) <---> Pin 10 (SCLK)
    Pin 6  (SSn)  <---> Pin 6  (SSn)

3. Before running the example, set a breakpoint at the GPIO_IntClear()
   statement in the main loop.  The buffer[] array contain 0s on initial
   execution, after which it should holds the values sent by the master
   (0 to 9 inclusive).

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG21A010F1024IM32

PA6 - USART0_TX (MOSI)  - Expansion Header pin 14
PA5 - USART0_RX (MISO)  - Expansion Header pin 12
PA3 - USART0_CLK (SCLK) - Expansion Header pin 10
PA1 - USART0_CS (SSn)   - Expansion Header pin 6