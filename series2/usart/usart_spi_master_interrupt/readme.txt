This project demonstrates interrupt-driven operation of the USART in
synchronous master mode.  USART0 is configured for SPI-compatible
operation at 1 Mbps.  Compatibility with the Synchronous Peripheral
Interface standard implies a word size of 8 data bits transmitted and
received MSB-first.

An output buffer is filled with 10 bytes of data (which can be changed by
modifying BUFLEN).  The USART chip select pin (controlled as a GPIO), is
asserted (driven low) to notify a slave device that data will soon be on
the way.

The first byte of data to be transmitted (outbuf[0]) is written to the
USART0_TXDATA register, after which the device enters the EM1 low-energy
mode.  While the CPU is halted, USART0 drives the CLK pin such that each
pulse causes the byte written to TXDATA to be shifted out while an
incoming byte is shifted into RXDATA one bit at a time.

Once the last CLK pulse has been driven, USART0 asserts its transmit
complete flag (USART0_STATUS_TXC) and its associated interrupt request
(USART0_IF_TXC).  This causes the processor to exit the EM1 low-energy
mode and enter the USART0_TX_IRQHandler() where the newly received byte
is written to the inbuf[] array.  If characters remain to be sent, the
next byte is written to TXDATA, and the TXC interrupt flag is cleared
before exiting the IRQ handler.

Until BUFLEN characters have been transferred, the device repeatedly
enters EM1, waiting for the transmit complete interrupt to wake it up in
order to save the most recently received character.

After BUFLEN characters have been transmitted and received, the chip
select is de-asserted (driven high), and the process repeats.  A
breakpoint can be set at the call to GPIO_PinOutSet() when the chip
selected is de-asserted to inspect the received data.

================================================================================

Peripherals Used:

GPIO
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

3. Before running the example, set a breakpoint on the GPIO_PinOutSet
   function call that de-asserts the slave select.  Examine the inbuf[]
   array to see the received contents from the slave.  If tested on a
   single WSTK with the USART0_RX (MISO) and USART0_TX (MOSI) pins
   connected together, inbuf[] will hold the same values as outbuf[].

   If tested using another WSTK running one of the slave examples,
   inbuf[] will hold all 0s after the first transmission sequence (slave
   select assertion, BUFLEN writes to USART0_TXDATA, and slave select
   de-assertion).  After each subsequent transmission, inbuf[] will hold
   the same values as outbuf[].

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG21A010F1024IM32

PA6 - USART0_TX (MOSI)  - Expansion Header pin 14
PA5 - USART0_RX (MISO)  - Expansion Header pin 12
PA3 - USART0_CLK (SCLK) - Expansion Header pin 10
PA1 - USART0_CS (SSn)   - Expansion Header pin 6