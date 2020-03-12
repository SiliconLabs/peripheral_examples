cryptoacc_entropy

This example uses the CRYPTOACC module in the EFR32 Series 2 to accelerate the 
mbed TLS entropy funtions for random number generation.

Note that mbed TLS APIs used in this project include alternative 
implementations(plugins) from Silicon Labs for some of the mbed TLS library 
functions, including AES, CCM, CMAC, ECC (ECP, ECDH, ECDSA, ECJPAKE), SHA1 and 
SHA256. The plugins use the AES and CRYPTO hardware modules to accelerate the 
standard mbed TLS library functions which are implemented in C. 

The user is expected to use this example only after installing the latest Gecko
SDK. Please refer to the mbed TLS section of the Gecko SDK documentation for 
more information on using mbed TLS on Silicon Labs devices. 

The True Random Number Generator (TRNG) in CRYPTOACC module is used as entropy
source. The entropy accumulator of mbed TLS will use SHA256 to hash the entropy
data pool which is filled with data from the entropy sources.

The entropy accumulator of mbed TLS is setup to use SHA-256 (MBEDTLS_ENTROPY_
FORCE_SHA256) which is accelerated by the CRYPTOACC SHA-256 accelerator.

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The user can press push button PB0 on WSTK to generate 32 bytes (256 bits)
random number which are printed to the VCOM serial interface.

The example has been instrumented with code to count the number of clock cycles
spent in generating random number. The results are printed to stdout, i.e. the
VCOM serial port console.

To check the performance gain of CRYPTOACC acceleration, the user can switch
off CRYPTOACC hardware acceleration by defining NO_CRYPTO_ACCELERATION symbol
in IDE setting.


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
4. Follow the instructions shown on the console.


Peripherals Used:
DC-DC
CRYPTOACC
HFXO   - 38.4 MHz
USART0 - 115200 baud, 8-N-1
GPIO

Board:  Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22C224F512IM40
PA05 - USART1 TX
PA06 - USART1 RX
PB04 - Board Controller VCOM Enable
PB00 - WSTK push button PB0