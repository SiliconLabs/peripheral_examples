usart_spi_secondary_polled

This project demonstrates polled operation of the USART in synchronous
secondary mode.  USART0 is configured for SPI-compatible operation at 1 Mbps.
Compatibility with the Synchronous Peripheral Interface standard implies
a word size of 8 data bits transmitted and received MSB-first.

It is important to note that because this example runs in secondary mode, the
GPIO configuration of the USART receive and transmit pins is reversed.
This is reflected in their SPI signal names: Main-In/Secondary Out
(MISO) and Main-Out/Secondary-In (MOSI).  So, while the USART_TX and
USART_RX pins are output and input, respectively, in main mode, they
operate as input (MOSI) and output (MISO) in secondary mode.

Likewise, the clock and chip select (secondary select) pins are also secondary
mode inputs.  Because a secondary device can be expected to operate primarily
in a low-power mode in order to reduce energy use, the chip select input
is configured to request a GPIO interrupt on a falling edge.  The main
device must add setup time after chip select assertion in order for the
secondary device to exit its low-power state and pre-load the transmit buffer
if it is expected to shift out a byte of data concurrent with the first
incoming byte fromt the main (even if this is otherwise ignored).  The
provided polled and interrupt-driven SPI main mode examples do this
with simple software delay loops, while the DMA-driven uses the USART's
timing control mechanism to do so.

A single output buffer is populated with 10 bytes (set by BUFLEN) of
outbound data that will be overwritten by the incoming data.  The device
polls for the falling edge of the chip select and then clears the
receive FIFO.  This step is not necessary in a well-behaved SPI system
configuration, but because this example is intended to run with one of the 
main mode examples, which constantly transmit data, it is required so
that data is received in the expected order when the main asserts the
chip select.

The specified number of data bytes are transmitted and received in a
for-loop.  The emlib USART_Tx() function checks for space in the transmit
FIFO space (USART_STATUS_TXBL != 0) before loading it with each byte.
USART_Rx() checks for valid receive data (USART_STATUS_RXDATAV == 1)
before reading a byte from the FIFO.  This provides the desired polling
mechanism, which is predicated on the main driving eight clock edges
for each byte transferred to and from the secondary.

Note: This example uses inclusive lexicon wherever possible. For more
information, visit https://www.silabs.com/about-us/inclusive-lexicon-project

================================================================================

Peripherals Used:

GPIO
USART0

The CMU is used indirectly via the USART_InitSync() function to calculate
the divisor necessary to derive the desired bit rate.

================================================================================

Test Procedure:

1. This example requires two Wireless Starter Kits (WSTKs).  Build one of
   the SPI main mode examples and download it to the first board.

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

4. Before running the example, set a breakpoint at the GPIO_IntClear()
   statement in the main loop.  The buffer[] array contain 0s on initial
   execution, after which it should holds the values sent by the main
   (0 to 9 inclusive).  The main example transfers data continuously
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
PC0  - USART0_TX (MOSI)  - Expansion Header pin 10
PC1  - USART0_RX (MISO)  - Expansion Header pin 4
PC2  - USART0_CLK (SCLK) - Expansion Header pin 6
PC3  - USART0_CS (CSn)   - Expansion Header pin 8

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
