usart_spi_main_polled

This project demonstrates polled operation of the USART in synchronous
main mode.  USART0 is configured for SPI-compatible operation at
1 Mbps.  Compatibility with the Synchronous Peripheral Interface standard
implies a word size of 8 data bits transmitted and received MSB-first.

An output buffer is filled with 10 bytes of data (which can be changed by
modifying BUFLEN).  The USART chip select pin (controlled as a GPIO), is
asserted (driven low) to notify a secondary device that data will soon be on
the way.  This example inserts a delay of around 15 us after chip select
assertion in order to allow the secondary device (expected to be one of the
equivalent EFM32/EFR32 interrupt-driven examples) to exit its low-power
state and pre-load its transmit buffer with the first byte of data to be
returned.

The emlib USART_SpiTransfer() function is used to transmit each byte and
save each byte that is simultaneously received.  USART_SpiTransfer()
polls the buffer level flag (USART_STATUS_TXBL), and, if space is
available, writes a byte to the transmit data register to be sent.  It
then polls the transmit complete flag (USART_STATUS_TXC) to determine
when all bits of this byte have been transmitted, which, in turn, denotes
that all bits of the incoming byte have been received.

After BUFLEN characters have been transmitted and received, the chip
select is de-asserted (driven high), and the process repeats.  A
breakpoint can be set at the call to GPIO_PinOutSet() when the chip
select is de-asserted to inspect the received data.

Before repeating the process, another delay (around 8 us) is inserted
because the secondary implementations running on slower devices (e.g. either
lower clock rate or lower IPC CPU such as the Cortex-M0+) need extra time
to prepare the input and output buffers for the next round of bytes to be
transferred.  Without this delay, these devices effectively skip
receiving data on every other chip select assertion.

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

1. This example requires two Starter Kits (STKs) for any two EFM32 or
   EFR32 devices.  Build the spi_secondary_polled example and download
   it to the first board.

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

4. Before running the example, set a breakpoint on the GPIO_PinOutSet
   function call that de-asserts the chip select.  Examine the inbuf[]
   array to see the received data from the secondary.  This example
   transfers data continuously but will not start until PB0 is pressed.

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
