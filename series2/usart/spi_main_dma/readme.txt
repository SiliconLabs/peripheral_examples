usart_spi_main_dma

This project demonstrates DMA-driven operation of the USART in
synchronous main mode.  USART0 is configured for SPI-compatible
operation at 1 Mbps.  Compatibility with the Synchronous Peripheral
Interface standard implies a word size of 8 data bits transmitted and
received MSB-first.

Processor intervention is minimized by enabling the USART's automatic
chip select assertion feature.  By default, the USART drives CLK 
concurrent with chip select assertion.  This lack of setup time may be
problematic depending on the secondary device's timing requirements.  The
secondary mode examples that can be run on another WSTK to verify operation
of this example need some amount of setup time to function properly.

To accommodate this, the autoCsSetup and autoCsHold members of the USART
synchronous initializer are set to 5.  This corresponds to the TCMP0
encoding, which inserts a user-specified delay (a) after chip select
assertion and before the first byte is transmitted and (b) after the last
byte is transmitted and before chip select de-assertion.  The length of
the delay ranges from 1 to 256 bit times and is specified by the value
written to the USART0->TIMECMP0_TCMPVAL field.

Buffers inbuf[] and outbuf[] are, respectively, zeroed out and populated
with 10 bytes of data (which can be changed by modifying BUFLEN).

Previously, the LDMA has been configured so that (a) one channel moves
2 bytes in outbuf[] to the USART0_TXDATA register when there is space
in the transmit FIFO (USART0_STATUS_TXBL is asserted) and (b) another
channel moves the matching received byte from USART0_RXDATA to inbuf[]
(upon assertion of USART0_STATUS_RXDATAV). TX channel transfers are
started when the transmit FIFO is empty, and RX channel transfers are
started when the receive FIFO is full. The device re-enters the EM1
low-energy mode after transfers are complete.

The transfer descriptor ignoreSrec and blockSize defaults are changed
so that the LDMA moves 2 bytes of data per request instead of 1
byte (ignoreSrec = 1). In this way, energy use is reduced because
each transfer of 2 bytes causes only one set of LDMA register accesses
to modify the working descriptors instead of doing so for each and every
byte transferred.

While the CPU is halted, the LDMA writes 2 bytes from outbuf[] to the 
transmit FIFO. USART0 drives the CLK pin so that each pulse causes the byte 
written to TXDATA to be shifted out while an incoming byte is shifted into 
RXDATA one bit at a time. When all 8 bits of the incoming byte have been 
received, the transmit and receive sequence is repeated because the LDMA moves 
2 bytes per transfer (ignoreSrec = 1 and blockSize = 2). Reception of the 
second byte results in a FIFO full condition, and the LDMA reads the 2 bytes 
in the receive FIFO and writes them to inbuf[].

Note: When BUFLEN is not a multiple of 2 (ldmaCtrlBlockSizeUnit2), the 
LDMA controller performs sequences of 2-unit (2-byte) transfers until the
last unit (byte) is remaining to be transferred and then it performs the 
last unit (byte) transfer at the end of the LDMA cycle.

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
USART0

The CMU is used indirectly via the USART_InitSync() function to calculate
the divisor necessary to derive the desired bit rate.

================================================================================

Test Procedure:

1. This example requires two Starter Kits (STKs) for any two EFM32 or
   EFR32 devices.  Build the spi_secondary_dma example and download it to
   the first board.

2. Build this project and download it to the second board.

3. Connect the Expansion Header pins from the main to the secondary board
   as follows:

     Main WSTK         Secondary WSTK
    -------------       -------------
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
   received data from the secondary.  This example transfers data continuously
   but will not start until PB0 is pressed.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 20 dBm Radio Board (BRD4180A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A020F1024IM32
PC00  - USART0_TX (MOSI)  - Expansion Header pin 4
PC01  - USART0_RX (MISO)  - Expansion Header pin 6
PC02  - USART0_CLK (SCLK) - Expansion Header pin 8
PC03  - USART0_CS (CSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PC00  - USART0_TX (MOSI)  - Expansion Header pin 4
PC01  - USART0_RX (MISO)  - Expansion Header pin 6
PC02  - USART0_CLK (SCLK) - Expansion Header pin 8
PC03  - USART0_CS (CSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22A224F512IM40
PC00  - USART0_TX (MOSI)  - Expansion Header pin 4
PC01  - USART0_RX (MISO)  - Expansion Header pin 6
PC02  - USART0_CLK (SCLK) - Expansion Header pin 8
PC03  - USART0_CS (CSn)   - Expansion Header pin 10

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PC00  - USART0_TX (MOSI)  - Expansion Header pin 10
PC01  - USART0_RX (MISO)  - Expansion Header pin 4
PC02  - USART0_CLK (SCLK) - Expansion Header pin 6
PC03  - USART0_CS (CSn)   - Expansion Header pin 8

Board:  Silicon Labs EFR32MG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48
PC00  - USART0_TX (MOSI)  - Expansion Header pin 10
PC01  - USART0_RX (MISO)  - Expansion Header pin 4
PC02  - USART0_CLK (SCLK) - Expansion Header pin 6
PC03  - USART0_CS (CSn)   - Expansion Header pin 8

Board:  Silicon Labs EFR32xG27 Buck Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PC00  - USART0_TX (MOSI)  - Expansion Header pin 4
PC01  - USART0_RX (MISO)  - Expansion Header pin 6
PC02  - USART0_CLK (SCLK) - Expansion Header pin 8
PC03  - USART0_CS (CSn)   - Expansion Header pin 10

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PD07  - USART0_TX (MOSI)  - Expansion Header pin 4
PD08  - USART0_RX (MISO)  - Expansion Header pin 6
PD09  - USART0_CLK (SCLK) - Expansion Header pin 8
PD10  - USART0_CS (CSn)   - Expansion Header pin 10
