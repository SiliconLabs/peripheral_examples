usart_spi_master_interrupt

This project demonstrates interrupt-driven operation of the USART in
synchronous master mode.  One of the USARTs (this depends on the device)
is configured for SPI-compatible operation at 1 Mbps.  Compatibility
with the Synchronous Peripheral Interface standard implies a word size
of 8 data bits transmitted and received MSB-first.

An output buffer is filled with 10 bytes of data (which can be changed by
modifying BUFLEN).  The USART chip select pin (controlled as a GPIO), is
asserted (driven low) to notify a slave device that data will soon be on
the way.  This example inserts a delay of around 15 us after chip select
assertion in order to allow the slave device (expected to be one of the
equivalent EFM32/EFR32 interrupt-driven examples) to exit its low-power
state and pre-load its transmit buffer with the first byte of data to be
returned.

The first byte of data to be transmitted (outbuf[0]) is written to the
USART_TXDATA register, after which the device enters the EM1 low-energy
mode.  While the CPU is halted, the USART drives its CLK pin such that
each pulse causes the byte written to TXDATA to be shifted out while an
incoming byte is shifted into RXDATA one bit at a time.

Once the last CLK pulse has been driven, the USART asserts its transmit
complete flag (USART_STATUS_TXC) and its associated interrupt request
(USART_IF_TXC).  This causes the processor to exit the EM1 low-energy
mode and enter the USART_TX_IRQHandler() where the newly received byte
is written to the inbuf[] array.  If characters remain to be sent, the
next byte is written to TXDATA, and the TXC interrupt flag is cleared
before exiting the IRQ handler.

Until BUFLEN characters have been transferred, the device repeatedly
enters EM1, waiting for the transmit complete interrupt to wake it up in
order to save the most recently received character.

After BUFLEN characters have been transmitted and received, the chip
select is de-asserted (driven high).  A breakpoint can be set at the
call to GPIO_PinOutSet() when the chip selected is de-asserted to
inspect the received data.

Before repeating the process, another delay (around 8 us) is inserted
because the slave implementations running on slower devices (e.g. either
lower clock rate or lower IPC CPU such as the Cortex-M0+) need extra time
to prepare the input and output buffers for the next round of bytes to be
transferred.  Without this delay, these devices effectively skip
receiving data on every other chip select assertion.

================================================================================

Peripherals Used:

GPIO
USART0 (Happy Gecko)
USART1 (all other Series 0 devices)

The CMU is used indirectly via the USART_InitSync() function to calculate
the divisor necessary to derive the desired bit rate.

================================================================================

Test Procedure:

1. This example requires two Starter Kits (STKs) for any two EFM32 or
   EFR32 devices.  Build the spi_slave_interrupt example and download
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

4. Before running the example, set a breakpoint on the GPIO_PinOutSet
   function call that de-asserts the chip select.  Examine the inbuf[]
   array to see the received data from the slave.  This example
   transfers data continuously but will not start until PB0 is pressed.

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: EFM32 Gecko Starter Kit (EFM32-G8XX-STK)

Device: EFM32G890F128

PD0 - USART1_TX (MOSI)  - Expansion Header pin 4
PD1 - USART1_RX (MISO)  - Expansion Header pin 6
PD2 - USART1_CLK (SCLK) - Expansion Header pin 8
PD3 - USART1_CS (SSn)   - Expansion Header pin 10

Board: EFM32 Giant Gecko Starter Kit (EFM32GG-STK3700A)

Device: EFM32GG990F1024

PD0 - USART1_TX (MOSI)  - Expansion Header pin 4
PD1 - USART1_RX (MISO)  - Expansion Header pin 6
PD2 - USART1_CLK (SCLK) - Expansion Header pin 8
PD3 - USART1_CS (SSn)   - Expansion Header pin 10

Board: EFM32 Happy Gecko Starter Kit (EFM32HG-STK3400A)

Device: EFM32HG322F64

PE10 - USART0_RX (MOSI)  - Expansion Header pin 4
PE11 - USART0_TX (MISO)  - Expansion Header pin 6
PE12 - USART0_CLK (SCLK) - Expansion Header pin 8
PE13 - USART0_CS (SSn)   - Expansion Header pin 10

Board: EFM32 Leopard Gecko Starter Kit (EFM32LG-STK3600A)

Device: EFM32LG990F256

PD0 - USART1_TX (MOSI)  - Expansion Header pin 4
PD1 - USART1_RX (MISO)  - Expansion Header pin 6
PD2 - USART1_CLK (SCLK) - Expansion Header pin 8
PD3 - USART1_CS (SSn)   - Expansion Header pin 10

Board: EFM32 Tiny Gecko Starter Kit (EFM32TG-STK3300A)

Device: EFM32TG840F32

PD0 - USART1_TX (MOSI)  - Expansion Header pin 4
PD1 - USART1_RX (MISO)  - Expansion Header pin 6
PD2 - USART1_CLK (SCLK) - Expansion Header pin 8
PD3 - USART1_CS (SSn)   - Expansion Header pin 10

Board: EFM32 Wonder Gecko Starter Kit (EFM32WG-STK3800A)

Device: EFM32WG990F256

PD0 - USART1_TX (MOSI)  - Expansion Header pin 4
PD1 - USART1_RX (MISO)  - Expansion Header pin 6
PD2 - USART1_CLK (SCLK) - Expansion Header pin 8
PD3 - USART1_CS (SSn)   - Expansion Header pin 10

Board: EFM32 Zero Gecko Starter Kit (EFM32ZG-STK3200A)

Device: EFM32ZG222F32

PD7  - USART1_RX (MOSI)  - Expansion Header pin 4
PD6  - USART1_TX (MISO)  - Expansion Header pin 6
PC15 - USART1_CLK (SCLK) - Expansion Header pin 8
PC14 - USART1_CS (SSn)   - Expansion Header pin 10
