eusart_spi_secondary_interrupt

This project demonstrates interrupt-driven operation of the EUSART in
synchronous secondary (formerly slave) mode.  EUSART0/1 is configured for
SPI-compatible operation at 1 Mbps.  Compatibility with the Synchronous
Peripheral Interface standard implies a word size of 8 data bits
transmitted and received MSB-first.

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
first incoming byte fromt the main (even if this is otherwise ignored).
The provided EUSART main mode examples do this with simple software delay
loops.

Buffers inbuf[] and outbuf[] are, respectively, zeroed out and populated
with 10 bytes of data (which can be changed by modifying BUFLEN).  The
falling edge interrupt is enabled for the chip select pin, and the device
waits in the EM1 low-energy mode.  Upon exit from EM1, the EUSART receiver
and transmitter are enabled in the event that the main was in mid-transfer
when the secondary started running.  This step is not necessary in a
well-behaved SPI system, but because this example is intended to run with
the matching main mode example, which constantly transmits data, it is
required so that data is received in the expected order when the main
asserts the chip select.

The first byte of data to be transmitted (outbuf[0]) is written to the
EUSART_TXDATA register, after which the device enters the EM1 low-energy
mode.  The EUSART_CLK pin is driven by the main so that each pulse
causes the byte written to TXDATA to be shifted out while an incoming
byte is shifted into RXDATA one bit at a time.  

Once the eighth CLK pulse of a byte has been received, the EUSART asserts
its receive FIFO level flag (EUSART_STATUS_RXFL) and its associated interrupt
request (EUSART_IF_RXFL).  This causes the processor to exit the EM1
low-energy mode and enter the EUSART_RX_IRQHandler() where the newly received
byte is written to the inbuf[] array.  If characters remain to be sent, the
next byte is written to TXDATA.  The RXFL interrupt flag is cleared before
exiting the IRQ handler.

Until BUFLEN characters have been transferred, the device repeatedly
enters EM1, waiting for the receive FIFO level interrupt to wake it up in
order to save the most recently received character.  After this, the EUSART
receive FIFO level and GPIO falling edge interrupts are disabled, and the
entire process repeats.

Note: This example uses inclusive lexicon wherever possible. For more
information, visit https://www.silabs.com/about-us/inclusive-lexicon-project

================================================================================

Peripherals Used:

GPIO
EUSART0/1

================================================================================

Test Procedure:

1. This example requires two Starter Kits (STKs) for any two EFM32 or
   EFR32 devices.  Build the spi_main_interrupt example and download
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

4. Start this example and then set a breakpoint at the while (1) in
   main().  By first starting the secondary and then setting the breakpoint,
   execution will stop after the first transfer of 10 bytes between the
   two boards.  The secondary example transfers data continuously but will
   not start until PB0 is pressed on the board running the main code.

5. When execution is halted, examine the inbuf[] array to see the
   received contents from the main; inbuf[] should hold the same values
   as outbuf[] given that all of the examples populate outbuf[] with the
   same data.

================================================================================

Activity Indicator

This example toggles a GPIO pin at critical points during execution so
that the timing of when the secondary is ready to transmit data or has
entered the receive data interrupt handler can be seen relative to the
start and transfer of data from the main.  On Series 2 devices, this is
always visible on expansion header pin 9.  The length of these pulses
is mostly dependent on the frequency of the HFCLK (and therefore the
number of wait states required to access flash) but is also impacted by
the device's CPU core (Cortex M0+ vs. M3/4 vs. M33) and the delays
through the pin-to-peripheral routing.

The sequence of pulses on the activity pin is documented in the code
comments and is as follows:

1. One pulse following chip select assertion by the main.  The rising
   edge of this pulse denotes exit from the GPIO IRQ handler and is the
   point at which the secondary enables the EUSART and RXDATAV interrupt
   and pre-loads the first byte to be sent to the main.  The falling edge
   of this pulse indicates that the secondary is ready to receive data.

2. The next BUFLEN pulses indicate entry into and exit from the EUSART RX
   IRQ handler.  In order to prevent drops, some amount of spacing is
   necessary between consecutive bytes when interrupt-driven.  The width
   of these pulses provides an idea of how much inter-byte spacing is
   necessary at a given clock rate.

3. After all data is sent, there will be a final, longer pulse during
   which the secondary prepares for the next transfer of BUFLEN bytes by
   pre-loading the outbuf[] array and zeroing out the inbuf[] array.

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
PD02 -  Activity Indicator - Expansion Header pin 9

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48 (this code will run unchanged on any
        EFR32MG24 variant so long as the pins used as present).
PC01 -  EUSART1_TX (MOSI)  - Expansion Header pin 4
PC02 -  EUSART1_RX (MISO)  - Expansion Header pin 6
PC03 -  EUSART1_CLK (SCLK) - Expansion Header pin 8
PC00 -  EUSART1_CS (CSn)   - Expansion Header pin 10
PD02 -  Activity Indicator - Expansion Header pin 9

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56 (this code will run unchanged on any
        EFR32FG25 variant so long as the pins used as present).
PC00 -  EUSART1_TX (MOSI)  - Expansion Header pin 4
PC01 -  EUSART1_RX (MISO)  - Expansion Header pin 6
PC02 -  EUSART1_CLK (SCLK) - Expansion Header pin 8
PB04 -  EUSART1_CS (CSn)   - Expansion Header pin 10
PA08 -  Activity Indicator - Expansion Header pin 12

Board:  Silicon Labs EFR32xG27 Buck Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PC00 -  EUSART0_TX (MOSI)  - Expansion Header pin 4
PC01 -  EUSART0_RX (MISO)  - Expansion Header pin 6
PC02 -  EUSART0_CLK (SCLK) - Expansion Header pin 8
PC03 -  EUSART0_CS (CSn)   - Expansion Header pin 10
PB01 -  Activity Indicator - Expansion Header pin 9

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PD07 -  EUSART1_TX (MOSI)  - Expansion Header pin 4
PD08 -  EUSART1_RX (MISO)  - Expansion Header pin 6
PD09 -  EUSART1_CLK (SCLK) - Expansion Header pin 8
PD10 -  EUSART1_CS (CSn)   - Expansion Header pin 10
PD11 -  Activity           - Expansion Header pin 12