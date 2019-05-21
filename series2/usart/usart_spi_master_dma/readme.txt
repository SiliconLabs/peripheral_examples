This project demonstrates DMA-driven operation of the USART in
synchronous master mode.  USART0 is configured for SPI-compatible
operation at 1 Mbps.  Compatibility with the Synchronous Peripheral
Interface standard implies a word size of 8 data bits transmitted and
received MSB-first.

Processor intervention is minimized by enabling the USART's automatic
chip select assertion feature.  By default, the USART drives CLK 
concurrent with chip select assertion.  This lack of setup time may be
problematic depending on the slave device's timing requirements.  In
particular, the polled and interrupt-driven slave examples that can be
run on another WSTK to verify operation of this example need some amount
of setup time to function properly.

To accommodate this, the autoCsSetup and autoCsHold members of the USART
synchronize initializer are set to 4.  This corresponds to 7 bit times of
delay inserted (a) after chip select assertion and before the first byte is
transmitted and (b) after the last byte is transmitted and before chip
select de-assertion.  Refer to the description of the USART_TIMING
register in the EFR32xG21 Reference Manual for more information.

Buffers inbuf[] and outbuf[] are, respectively, zeroed out and populated
with 10 bytes of data (which can be changed by modifying BUFLEN).

Previously, the LDMA has been configured so that (a) one channel moves
each byte in outbuf[] to the USART0_TXDATA register when there is space
in the transmit FIFO (USART0_STATUS_TXBL is asserted), and (b) another
channel moves the matching received byte from USART0_RXDATA to inbuf[]
(upon assertion of USART0_STATUS_RXDATAV).  Transfers are started for
these channels, and the device enters the EM1 low-energy mode.

While the CPU is halted, the LDMA writes each byte from outbuf[] to
the transmit FIFO as space is available.  USART0 drives the CLK pin
so that each pulse causes the byte written to TXDATA to be shifted out
while an incoming byte is shifted into RXDATA one bit at a time.  Once
all 8 bits of the incoming byte have been received, the LDMA writes each
byte from the receive FIFO to inbuf[].

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

1. Build the project and download it to the Wirelss Starter Kit (WSTK).

2a. If only a single WSTK is available, connect pins 12 and 14 on the
    Expansion Header with a jumper wire.  These are the USART0_RX (MISO)
    and USART0_TX (MOSI) pins, such that connecting them results in the
    master receiving the data it jus transmitted.

2b. If another WSTK is available, compile and download one of the slave
    mode examples to the other board and connect the Expansion Header
    pins as follows:

     Master WSTK         Slave WSTK
    -------------       -------------
    Pin 1  (GND)  <---> Pin 1 (GND)
    Pin 14 (MOSI) <---> Pin 14 (MOSI)
    Pin 12 (MISO) <---> Pin 12 (MISO)
    Pin 10 (SCLK) <---> Pin 10 (SCLK)
    Pin 6  (SSn)  <---> Pin 6  (SSn)

3. Before running the example, set a breakpoint on the rx_done = 0
   statement in the main loop.  Examine the inbuf[] array to see the
   received contents from the slave.  If tested on a single WSTK with the
   USART0_RX (MISO) and USART0_TX (MOSI) pins connected together, inbuf[]
   will hold the same values as outbuf[].

   If tested using another WSTK running one of the slave examples,
   inbuf[] holds all 0s after the first set of transfers and then the
   same values as outbuf[] after each subsequent iteration.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG21A010F1024IM32

PA6 - USART0_TX (MOSI)  - Expansion Header pin 14
PA5 - USART0_RX (MISO)  - Expansion Header pin 12
PA3 - USART0_CLK (SCLK) - Expansion Header pin 10
PA1 - USART0_CS (SSn)   - Expansion Header pin 6