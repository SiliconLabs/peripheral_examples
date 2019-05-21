This project demonstrates interrupt-driven operation of the USART in
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

The first byte of data to be transmitted (outbuf[0]) is written to the
USART0_TXDATA register, after which the device enters the EM1 low-energy
mode.  The USART0_CLK pin is driven by the master so that each pulse
causes the byte written to TXDATA to be shifted out while an incoming
byte is shifted into RXDATA one bit at a time.  

Once the eighth CLK pulse of a byte has been received, USART0 asserts its
receive data valid flag (USART0_STATUS_RXDATAV) and its associated
interrupt request (USART0_IF_RXDATAV).  This causes the processor to exit
the EM1 low-energy mode and enter the USART0_RX_IRQHandler() where the
newly received byte is written to the inbuf[] array.  If characters remain
to be sent, the next byte is written to TXDATA, and the RXDATAV interrupt
flag is cleared before exiting the IRQ handler.

Until BUFLEN characters have been transferred, the device repeatedly
enters EM1, waiting for the receive data valid interrupt to wake it up in
order to save the most recently received character.  After this, the
USART0 receive data valid and GPIO falling edge interrupts are disabled,
and the entire process repeats.

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

4. Before running the example, set a breakpoint at the GPIO_IntDisable()
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