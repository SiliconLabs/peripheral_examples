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
USART2 (EFM32PG12 and EFR32xG12)
USART1 (EFM32PG1, EFR32xG1, EFR32xG13, and EFR32xG14)
USART0 (EFM32GG11 and EFM32TG11)

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

Board: Silicon Labs EFM32 Giant Gecko GG11 Starter Kit (SLSTK3701A)

Device: EFM32GG11B820F2048GL192

PE10 - USART0_TX (MOSI)  - Expansion Header pin 4
PE11 - USART0_RX (MISO)  - Expansion Header pin 6
PE12 - USART0_CLK (SCLK) - Expansion Header pin 8
PE14 - USART0_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFM32 Pearl Gecko PG1 Starter Kit (SLSTK3401A)

Device: EFM32PG1B200F256GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFM32 Pearl Gecko PG12 Starter Kit (SLSTK3402A)

Device: EFM32PG12B500F1024GL125

PA6 - USART2_TX (MOSI)  - Expansion Header pin 4
PA7 - USART2_RX (MISO)  - Expansion Header pin 6
PA8 - USART2_CLK (SCLK) - Expansion Header pin 8
PA9 - USART2_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFM32 Tiny Gecko TG11 Starter Kit (SLSTK3301A)

Device: EFM32TG11B520F128GM80

PC11 - USART0_TX (MOSI)  - Expansion Header pin 4
PC10 - USART0_RX (MISO)  - Expansion Header pin 6
PA12 - USART0_CLK (SCLK) - Expansion Header pin 8
PC8  - USART0_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32BG1 2.4 GHz 10.5 dBm Radio Board (BRD4100A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32BG1P232F256GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32BG12 2.4 GHz 10 dBm Radio Board (BRD4103A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32BG12P332F1024GL125

PA6 - USART2_TX (MOSI)  - Expansion Header pin 4
PA7 - USART2_RX (MISO)  - Expansion Header pin 6
PA8 - USART2_CLK (SCLK) - Expansion Header pin 8
PA9 - USART2_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32BG13 2.4 GHz 10 dBm Radio Board (BRD4104A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32BG13P632F512GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32FG1 2.4 GHz / 915 MHz Dual Band 19.5 dBm Radio Board (BRD4250A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32FG1P133F256GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32FG12 2.4 GHz / 915 MHz Dual Band 19 dBm Radio Board (BRD4253A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32FG12P433F1024GL125

PA6 - USART2_TX (MOSI)  - Expansion Header pin 4
PA7 - USART2_RX (MISO)  - Expansion Header pin 6
PA8 - USART2_CLK (SCLK) - Expansion Header pin 8
PA9 - USART2_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32FG13 2.4 GHz / 915 MHz Dual Band 19 dBm Radio Board (BRD4255A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32FG13P233F512GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32FG13 2.4 GHz / 868 MHz Dual Band 10 dBm Radio Board (BRD4256A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32FG13P233F512GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32FG14 2.4 GHz / 915 MHz Dual Band 19 dBm Radio Board (BRD4257A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32FG14P233F256GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32MG1 2.4 GHz 19.5 dBm Radio Board (BRD4151A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG1P232F256GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32MG12 2.4 GHz 10 dBm Radio Board (BRD4162A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG12P332F1024GL125

PA6 - USART2_TX (MOSI)  - Expansion Header pin 4
PA7 - USART2_RX (MISO)  - Expansion Header pin 6
PA8 - USART2_CLK (SCLK) - Expansion Header pin 8
PA9 - USART2_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32MG12 2.4 GHz/915 MHz Dual Band 19 dBm Radio Board (BRD4164A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG12P433F1024GL125

PA6 - USART2_TX (MOSI)  - Expansion Header pin 4
PA7 - USART2_RX (MISO)  - Expansion Header pin 6
PA8 - USART2_CLK (SCLK) - Expansion Header pin 8
PA9 - USART2_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32MG13 2.4 GHz 10 dBm Radio Board (BRD4159A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG13P632F512GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32MG13 2.4 GHz/915 MHz Dual Band 19 dBm Radio Board (BRD4158A)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG13P733F512GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10

Board: Silicon Labs EFR32MG14 2.4 GHz/915 MHz Dual Band 19 dBm Radio Board (BRD4169B)
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG14P733F256GM48

PC6 - USART1_TX (MOSI)  - Expansion Header pin 4
PC7 - USART1_RX (MISO)  - Expansion Header pin 6
PC8 - USART1_CLK (SCLK) - Expansion Header pin 8
PC9 - USART1_CS (SSn)   - Expansion Header pin 10
