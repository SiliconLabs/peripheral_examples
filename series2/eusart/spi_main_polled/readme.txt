eusart_spi_main_polled

This project demonstrates polled operation of the EUSART in synchronous
main (formerly master) mode.  EUSART0/1 is configured for SPI-compatible
operation at 1 Mbps.  Compatibility with the Synchronous Peripheral
Interface standard implies a word size of 8 data bits transmitted and
received MSB-first.

An output buffer is filled with 10 bytes of data (which can be changed by
modifying BUFLEN).  The EUSART chip select pin (controlled as a GPIO), is
asserted (driven low) to notify a secondary (formerly slave) device that
data will soon be on the way.  This example inserts a delay of around 15 us
after chip select assertion in order to allow the secondary device (expected
to be one of the equivalent EFM32/EFR32 interrupt-driven examples) to exit
its low-power state and pre-load its transmit buffer with the first byte of
data to be returned.

The emlib EUSART_Spi_TxRx() function is used to transmit each byte and
save each byte that is simultaneously received.  EUSART_Spi_TxRx() polls
the FIFO level flag (EUSART_STATUS_TXFL), and, if space is available,
writes a byte to the transmit data register to be sent.  It then polls
the receive FIFO level flag (EUSART_STATUS_RXFL) to determine when all
bits of the incoming frame have been received, which, in turn.

After BUFLEN characters have been transmitted and received, the chip
select is de-asserted (driven high), and the process repeats.  A
breakpoint can be set at the call to GPIO_PinOutSet() when the chip
select is de-asserted to inspect the received data.

Before repeating the process, another delay (around 8 us) is inserted
because the secondary implementations running on slower devices (e.g.
either lower clock rate or lower IPC CPU such as the Cortex-M0+) need
extra time to prepare the input and output buffers for the next round of
bytes to be transferred.  Without this delay, these devices effectively
skip receiving data on every other chip select assertion.

Note: This example uses inclusive lexicon wherever possible. For more
information, visit https://www.silabs.com/about-us/inclusive-lexicon-project

================================================================================

Peripherals Used:

GPIO
EUSART0/1

The CMU is used indirectly via the EUSART_SpiInit() function to calculate
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
    -------------       --------------
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

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48 (this code will run unchanged on any
        EFR32ZG23 variant so long as the pins used as present).
PC01 -  EUSART1_TX (MOSI)  - Expansion Header pin 4
PC02 -  EUSART1_RX (MISO)  - Expansion Header pin 6
PC03 -  EUSART1_CLK (SCLK) - Expansion Header pin 8
PC00 -  EUSART1_CS (CSn)   - Expansion Header pin 10

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48 (this code will run unchanged on any
        EFR32MG24 variant so long as the pins used as present).
PC01 -  EUSART1_TX (MOSI)  - Expansion Header pin 4
PC02 -  EUSART1_RX (MISO)  - Expansion Header pin 6
PC03 -  EUSART1_CLK (SCLK) - Expansion Header pin 8
PC00 -  EUSART1_CS (CSn)   - Expansion Header pin 10

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56 (this code will run unchanged on any
        EFR32FG25 variant so long as the pins used as present).
PC00 -  EUSART1_TX (MOSI)  - Expansion Header pin 4
PC01 -  EUSART1_RX (MISO)  - Expansion Header pin 6
PC02 -  EUSART1_CLK (SCLK) - Expansion Header pin 8
PB04 -  EUSART1_CS (CSn)   - Expansion Header pin 10

Board:  Silicon Labs EFR32xG27 Buck Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PC00 -  EUSART0_TX (MOSI)  - Expansion Header pin 4
PC01 -  EUSART0_RX (MISO)  - Expansion Header pin 6
PC02 -  EUSART0_CLK (SCLK) - Expansion Header pin 8
PC03 -  EUSART0_CS (CSn)   - Expansion Header pin 10

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PD07 -  EUSART1_TX (MOSI)  - Expansion Header pin 4
PD08 -  EUSART1_RX (MISO)  - Expansion Header pin 6
PD09 -  EUSART1_CLK (SCLK) - Expansion Header pin 8
PD10 -  EUSART1_CS (CSn)   - Expansion Header pin 10
