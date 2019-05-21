se_entropy

This example uses the CRYPTO and TRNG module in the EFR32 Series 2 Secure
Element (SE) to accelerate the mbed TLS entropy funtions for random number
generation.

The True Random Number Generator (TRNG) hardware module is used as entropy
source. The entropy accumulator of mbed TLS will use SHA256 to hash the entropy
data pool which is filled with data from the entropy sources.

The entropy accumulator of mbed TLS is setup to use SHA-256 (MBEDTLS_ENTROPY_
FORCE_SHA256) which is accelerated by the CRYPTO SHA-256 accelerator.

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The user can press push button PB0 on WSTK to generate 32 bytes (256 bits)
random number which are printed to the VCOM serial interface.

The example has been instrumented with code to count the number of clock cycles
spent in generating random number. The results are printed to stdout, i.e. the
VCOM serial port console.

To check the performance gain of CRYPTO acceleration, the user can switch off
CRYPTO hardware acceleration by defining NO_CRYPTO_ACCELERATION symbol in IDE
setting.


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
4. Follow the instructions shown on the console.


Peripherals Used:
HFXO   - 38.4 MHz
USART0 - 115200 baud, 8-N-1
GPIO
SE

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21A010F1024IM32
PA05 - USART0 TX
PA06 - USART0 RX
PD04 - Board Controller VCOM Enable
PD02 - WSTK push button PB0