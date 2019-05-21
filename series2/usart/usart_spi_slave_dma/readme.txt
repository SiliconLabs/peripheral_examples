This project demonstrates DMA-driven operation of the USART in
synchronous slave mode.  USART0 is configured for SPI-compatible
operation at 1 Mbps.  Compatibility with the Synchronous Peripheral
Interface standard implies a word size of 8 data bits transmitted and
received MSB-first.

It is important to note that because this example runs in slave mode, the
GPIO configuration of the USART receive and transmit pins is reversed.
This is reflected in their SPI signal names: Master-In/Slave Out
(MISO) and Master-Out/Slave-In (MOSI).  So, while the USART_TX and
USART_RX pins are output and input, respectively, in master mode, they
operate as input (MOSI) and output (MISO) in slave mode.

Likewise, the clock and chip select (slave select) pins are also slave
mode inputs.  Because a slave device can be expected to operate primarily
in a low-power mode in order to reduce energy use, the chip select input
is configured to request a GPIO interrupt on a falling edge.  The master
device must add setup time after chip select assertion in order for the
slave device to exit its low-power state and pre-load the transmit buffer
if it is expected to shift out a byte of data concurrent with the first
incoming byte fromt the master (even if this is otherwise ignored).  The
provided polled and interrupt-driven SPI master mode examples do this
simply by manually both asserting the chip select and loading the first
byte of data to be transmitted.  The DMA-driven master example includes
setup time insertion through use of the auto chip select assertion
mechanism.

Buffers inbuf[] and outbuf[] are, respectively, zeroed out and populated
with 10 bytes of data (which can be changed by modifying BUFLEN).  The
falling edge interrupt is enabled for the chip select pin, and the device
waits in the EM1 low-energy mode.  Upon exit from EM1 the receive FIFO is
cleared.  This step is not necessary in a well-behaved SPI system, but
because this example is intended to run with one of the master mode
examples, which constantly transmit data, it is required so that data is
received in the expected order when the master asserts the chip select.

Previously, the LDMA has been configured so that (a) one channel moves
each byte in outbuf[] to the USART0_TXDATA register when there is space
in the transmit FIFO (USART0_STATUS_TXBL is asserted), and (b) another
channel moves the matching received byte from USART0_RXDATA to inbuf[]
(upon assertion of USART0_STATUS_RXDATAV).  Transfers are started for
these channels, and the device enters the EM1 low-energy mode.

While the CPU is halted, the LDMA writes each byte from outbuf[] to
the transmit FIFO as space is available.  The USART0_CLK pin is driven by
the master so that each pulse causes the byte written to TXDATA to be
shifted out while an incoming byte is shifted into RXDATA one bit at a
time.  Once all 8 bits of the incoming byte have been received, the LDMA
writes each byte from the receive FIFO to inbuf[].

This process occurs autonomously, and only the LDMA interrupt request
(assertion of one or more of the LDMA_IF_DONEx flags or LDMA_IF_ERROR in
the event of some kind of problem) causes the device to exit EM1.  The
LDMA_IRQHandler() clears each channel done interrupt flag and sets the
rx_done state variable to indicate transfer completion when all bytes
have been received.  Set a breakpoint in the main loop where the rx_done
flag is cleared in order to inspect the contents of inbuf[] before and
after each set of transfers.

================================================================================

Peripherals Used:

GPIO
LDMA
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

4. Before running the example, set a breakpoint on the rx_done = 0
   statement in the main loop.  Examine the inbuf[] array to see the
   received contents from the master; inbuf[] should hold the same values
   as outbuf[] given that all of the examples populate outbuf[] with the
   same data.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG21A010F1024IM32

PA6 - USART0_TX (MOSI)  - Expansion Header pin 14
PA5 - USART0_RX (MISO)  - Expansion Header pin 12
PA3 - USART0_CLK (SCLK) - Expansion Header pin 10
PA1 - USART0_CS (SSn)   - Expansion Header pin 6