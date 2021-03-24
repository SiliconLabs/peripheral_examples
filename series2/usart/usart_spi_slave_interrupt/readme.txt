usart_spi_slave_interrupt

This project demonstrates interrupt-driven operation of the USART in
synchronous slave mode.  One of the USARTs (this depends on the device)
is configured for SPI-compatible operation at 1 Mbps.  Compatibility
with the Synchronous Peripheral Interface standard implies a word size
of 8 data bits transmitted and received MSB-first.

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
matching spi_master_interrupt examples do this with a software loop that
inserts a delay of around 15 us after asserting the chip select but
before loading the first byte of data to be transmitted.

Buffers inbuf[] and outbuf[] are, respectively, zeroed out and populated
with 10 bytes of data (which can be changed by modifying BUFLEN).  The
falling edge interrupt is enabled for the chip select pin, and the device
waits in the EM1 low-energy mode.  Upon exit from EM1, the USART receiver
and transmitter are enabled in the event that the master was in
mid-transfer when the slave started running.  This step is not necessary
in a well-behaved SPI system, but because this example is intended to run
with the matching master mode example, which constantly transmits data,
it is required so that data is received in the expected order when the
master asserts the chip select.

The first byte of data to be transmitted (outbuf[0]) is written to the
USART_TXDATA register, after which the device enters the EM1 low-energy
mode.  The USART_CLK pin is driven by the master so that each pulse
causes the byte written to TXDATA to be shifted out while an incoming
byte is shifted into RXDATA one bit at a time.  

Once the eighth CLK pulse of a byte has been received, the USART asserts
its receive data valid flag (USART_STATUS_RXDATAV) and its associated
interrupt request (USART_IF_RXDATAV).  This causes the processor to exit
the EM1 low-energy mode and enter the USART_RX_IRQHandler() where the
newly received byte is written to the inbuf[] array.  If characters remain
to be sent, the next byte is written to TXDATA.  The RXDATAV interrupt
flag is cleared before exiting the IRQ handler by dint of reading the
RXDATA register.

Until BUFLEN characters have been transferred, the device repeatedly
enters EM1, waiting for the receive data valid interrupt to wake it up in
order to save the most recently received character.  After this, the
USART receive data valid and GPIO falling edge interrupts are disabled,
and the entire process repeats.

================================================================================

Peripherals Used:

GPIO
USART0

================================================================================

Test Procedure:

1. This example requires two Starter Kits (STKs) for any two EFM32 or
   EFR32 devices.  Build the spi_master_interrupt example and download
   it to the first board.

2. Build this project and download it to the second board.

3. Connect the Expansion Header pins from the master to the slave board
   as follows:

     Master WSTK         Slave WSTK
    -------------       -------------
    Pin 1  (GND)  <---> Pin 1  (GND)
    Pin 4  (MOSI) <---> Pin 4  (MOSI)
    Pin 6  (MISO) <---> Pin 6  (MISO)
    Pin 8  (SCLK) <---> Pin 8  (SCLK)
    Pin 10 (SSn)  <---> Pin 10 (SSn)

4. Start this example and then set a breakpoint at the while (1) in
   main().  By first starting the slave and then setting the breakpoint,
   execution will stop after the first transfer of 10 bytes between the
   two boards.  The master example transfers data continuously but will
   not start until PB0 is pressed.

5. When execution is halted, examine the inbuf[] array to see the
   received contents from the master; inbuf[] should hold the same values
   as outbuf[] given that all of the examples populate outbuf[] with the
   same data.

================================================================================

Activity Indicator

This example toggles a GPIO pin at critical points during execution so
that the timing of when the slave is ready to transmit data or has
entered the receive data interrupt handler can be seen relative to the
start and transfer of data from the master.  On Series 2 devices, this
is always visible on expansion header pin 12.  The length of these
pulses is mostly dependent on the frequency of the HFCLK (and therefore
the number of wait states required to access flash) but is also impacted
by the device's CPU core (Cortex M0+ vs. M3 vs. M4) and the delays
through the pin-to-peripheral routing.

The sequence of pulses on the activity pin is documented in the code
comments and is as follows:

1. One pulse following chip select assertion by the master.  The rising
   edge of this pulse denotes exit from the GPIO IRQ handler and is the
   point at which the slave enables the USART and RXDATAV interrupt and
   pre-loads the first byte to be sent to the master.  The falling edge
   of this pulse indicates that the slave is ready to receive data.

2. The next BUFLEN pulses indicate entry into and exit from the USART RX
   IRQ handler.  In order to prevent drops, some amount of spacing is
   necessary between consecutive bytes when interrupt-driven.  The width
   of these pulses provides an idea of how much inter-byte spacing is
   necessary at a given clock rate.

3. After all data is sent, there will be a final, longer pulse during
   which the slave prepares for the next transfer of BUFLEN bytes by
   pre-loading the outbuf[] array and zeroing out the inbuf[] array.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 20 dBm Board (BRD4180A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG21A020F1024IM32

PC0 - USART1_TX (MOSI)  - Expansion Header pin 4
PC1 - USART1_RX (MISO)  - Expansion Header pin 6
PC2 - USART1_CLK (SCLK) - Expansion Header pin 8
PC3 - USART1_CS (SSn)   - Expansion Header pin 10
PA5 - Activity          - Expansion Header pin 12

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG21A010F1024IM32

PC0 - USART1_TX (MOSI)  - Expansion Header pin 4
PC1 - USART1_RX (MISO)  - Expansion Header pin 6
PC2 - USART1_CLK (SCLK) - Expansion Header pin 8
PC3 - USART1_CS (SSn)   - Expansion Header pin 10
PA5 - Activity          - Expansion Header pin 12

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG22A224F512IM40

PC0 - USART1_TX (MOSI)  - Expansion Header pin 4
PC1 - USART1_RX (MISO)  - Expansion Header pin 6
PC2 - USART1_CLK (SCLK) - Expansion Header pin 8
PC3 - USART1_CS (SSn)   - Expansion Header pin 10
PA5 - Activity          - Expansion Header pin 12
