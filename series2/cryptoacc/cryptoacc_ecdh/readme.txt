cryptoacc_ecdh

This example uses the CRYPTOACC module in the EFR32 Series 2 to accelerate the
ECDH API functions of mbed TLS.

Note that mbed TLS APIs used in this project include alternative 
implementations(plugins) from Silicon Labs for some of the mbed TLS library 
functions, including AES, CCM, CMAC, ECC (ECP, ECDH, ECDSA, ECJPAKE), SHA1 and 
SHA256. The plugins use the AES and CRYPTO hardware modules to accelerate the 
standard mbed TLS library functions which are implemented in C. 

The user is expected to use this example only after installing the latest Gecko
SDK. Please refer to the mbed TLS section of the Gecko SDK documentation for 
more information on using mbed TLS on Silicon Labs devices. 

The Elliptic Curve Diffie Hellman (ECDH) is an anonymous key agreement protocol
that allows two parties, each having an elliptic-curve public-private key pair,
to establish a shared secret over an insecure channel.

The example uses the CTR-DRBG, a pseudo random number generator (PRNG) included
in mbed TLS to generate random private keys for both peers (client and server)
that will share the ECDH secret. The True Random Number Generator (TRNG)
hardware module is used as entropy source to seed the CTR-DRBG. The entropy 
accumulator of mbed TLS will use SHA256 to hash the entropy data pool which is
filled with data from the entropy sources.

The CTR-DRBG involves use of AES which is accelerated by the CRYPTOACC AES
accelerator.

The entropy accumulator of mbed TLS is setup to use SHA-256 (MBEDTLS_ENTROPY_
FORCE_SHA256) which is accelerated by the CRYPTOACC SHA-256 accelerator.

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent inside the ECDH API calls, mbedtls_ctr_drbg_seed, mbedtls_ecdh_gen_public
and mbedtls_ecdh_compute_shared. The results are printed to stdout, i.e. the
VCOM serial port console.

To check the performance gain of CRYPTOACC acceleration, the user can switch
off CRYPTOACC hardware acceleration by defining NO_CRYPTO_ACCELERATION symbol
in IDE setting.

The user can change the specific ECC curve used in the example by modifying the
MBEDTLS_ECC_ID macro definition. Available curves with CRYPTOACC acceleration
support are:
MBEDTLS_ECP_DP_SECP256R1 (default)
MBEDTLS_ECP_DP_SECP192R1
MBEDTLS_ECP_DP_SECP224R1

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
4. The terminal screen should display the process steps of ECDH.


Peripherals Used:
DC-DC
CRYPTOACC
HFXO   - 38.4 MHz
USART0 - 115200 baud, 8-N-1

Board:  Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22C224F512IM40
PA05 - USART1 TX
PA06 - USART1 RX
PB04 - Board Controller VCOM Enable