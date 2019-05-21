se_ecdh

This example uses the CRYPTO module in the EFR32 Series 2 Secure Element (SE) to
accelerate the ECDH API functions of mbed TLS.

The Elliptic Curve Diffie Hellman (ECDH) is an anonymous key agreement protocol
that allows two parties, each having an elliptic-curve public-private key pair,
to establish a shared secret over an insecure channel.

The example uses the CTR-DRBG, a pseudo random number generator (PRNG) included
in mbed TLS to generate random private keys for both peers (client and server)
that will share the ECDH secret. The True Random Number Generator (TRNG)
hardware module is used as entropy source to seed the CTR-DRBG. The entropy 
accumulator of mbed TLS will use SHA256 to hash the entropy data pool which is
filled with data from the entropy sources.

The CTR-DRBG involves use of AES which is accelerated by the CRYPTO AES
accelerator.

The entropy accumulator of mbed TLS is setup to use SHA-256 (MBEDTLS_ENTROPY_
FORCE_SHA256) which is accelerated by the CRYPTO SHA-256 accelerator.

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent inside the ECDH API calls, mbedtls_ctr_drbg_seed, mbedtls_ecdh_gen_public
and mbedtls_ecdh_compute_shared. The results are printed to stdout, i.e. the
VCOM serial port console.

To check the performance gain of CRYPTO acceleration, the user can switch off
CRYPTO hardware acceleration by defining NO_CRYPTO_ACCELERATION symbol in IDE
setting.

The user can change the specific ECC curve used in the example by modifying the
MBEDTLS_ECC_ID macro definition. Available curves with CRYPTO acceleration
support are:
MBEDTLS_ECP_DP_SECP256R1 (default)
MBEDTLS_ECP_DP_SECP192R1


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
4. The terminal screen should display the process steps of ECDH.


Peripherals Used:
HFXO   - 38.4 MHz
USART0 - 115200 baud, 8-N-1
SE

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21A010F1024IM32
PA05 - USART0 TX
PA06 - USART0 RX
PD04 - Board Controller VCOM Enable