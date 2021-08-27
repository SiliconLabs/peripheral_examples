eusart_spi_secondary_dma

This project demonstrates DMA-driven operation of the EUSART in
synchronous secondary (formerly slave) mode.  EUSART1 is configured
for SPI-compatible operation at 1 Mbps.  Compatibility with the
Synchronous Peripheral Interface standard implies a word size of 8
data bits transmitted and received MSB-first.

It is important to note that because this example runs in secondary mode,
the GPIO configuration of the EUSART receive and transmit pins is reversed.
This is reflected in their SPI signal names: Main In/Secondary Out
(MISO) and Main Out/Secondary In (MOSI).  Thus, in main mode the EUSART_TX
and EUSART_RX pins are output and input, respectively, while in secondary
mode they operate as input (MOSI) and output (MISO), respectively.

Likewise, the clock and chip select pins are also secondary mode inputs.
Because a secondary device can be expected to operate primarily in a
low-power mode in order to reduce energy use, the chip select input
is configured to request a GPIO interrupt on a falling edge.  The main
device must add setup time after chip select assertion in order for the
secondary device to exit its low-power state and pre-load the transmit
buffer if it is expected to shift out a byte of data concurrent with the
first incoming byte from the main (even if this is otherwise ignored).
The provided EUSART main mode examples do this with simple software delay
loops.

Buffers inbuf[] and outbuf[] are, respectively, zeroed out and populated
with 10 bytes of data (which can be changed by modifying BUFLEN).  The
falling edge interrupt is enabled for the chip select pin, and the device
waits in the EM1 low-energy mode.  Upon exit from EM1 the receive FIFO is
cleared.  This step is not necessary in a well-behaved SPI system, but
because this example is intended to run with one of the main mode
examples, which constantly transmit data, it is required so that data is
received in the expected order when the main asserts the chip select.

Previously, the LDMA has been configured so that (a) one channel moves
each byte in outbuf[] to the EUSART1_TXDATA register when there is space
in the transmit FIFO (EUSART1_STATUS_TXFL is asserted), and (b) another
channel moves the matching received byte from EUSART1_RXDATA to inbuf[]
(upon assertion of EUSART1_STATUS_RXFL).  Transfers are started for
these channels, and the device enters the EM1 low-energy mode.

While the CPU is halted, the LDMA writes each byte from outbuf[] to
the transmit FIFO as space is available.  The EUSART1_CLK pin is driven by
the main so that each pulse causes the byte written to TXDATA to be
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

Note: This example uses inclusive lexicon wherever possible. For more
information, visit https://www.silabs.com/about-us/inclusive-lexicon-project

================================================================================

Peripherals Used:

GPIO
LDMA
EUSART1

================================================================================

Test Procedure:

1. This example requires two Wireless Starter Kits (WSTKs).  Build one of
   the SPI main mode examples and download it to the first board.

2. Build this project and download it to the second board.

3. Connect the Expansion Header pins from the main to the secondary board
   as follows:

      Main WSTK         Secondary WSTK
    -------------       --------------
    Pin 1  (GND)  <---> Pin 1  (GND)
    Pin 4  (MOSI) <---> Pin 4  (MOSI)
    Pin 6  (MISO) <---> Pin 6  (MISO)
    Pin 8  (SCLK) <---> Pin 8  (SCLK)
    Pin 10 (CSn)  <---> Pin 10 (CSn)

   NOTE: Some radio boards do not follow this convention for SPI
   pin-mapping.  See the supported board list below for specific
   assignments.

4. Before running the example, set a breakpoint on the rx_done = false
   statement in the main loop.  Examine the inbuf[] array to see the
   received data from the main.  This example transfers data continuously
   but will not start until PB0 is pressed on the board running the main code.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32FG23 868-915 MHz 14 dBm Radio Board (BRD4263B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG23A010F512GM48 (this code will run unchanged on any
        EFR32FG23 variant so long as the pins used as present).
PC1 - EUSART1_TX (MOSI)  - Expansion Header pin 4
PC2 - EUSART1_RX (MISO)  - Expansion Header pin 6
PC3 - EUSART1_CLK (SCLK) - Expansion Header pin 8
PC0 - EUSART1_CS (CSn)   - Expansion Header pin 10