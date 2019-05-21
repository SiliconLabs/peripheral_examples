se_ecdsa

This example uses the CRYPTO module in the EFR32 Series 2 Secure Element (SE) to
accelerate the ECDSA API functions of mbed TLS.

The example uses the CTR-DRBG, a pseudo random number generator (PRNG) included
in mbed TLS to generate random key pair for signing. The True Random Number
Generator (TRNG) hardware module is used as entropy source to seed the CTR-DRBG.
The entropy accumulator of mbed TLS will use SHA256 to hash the entropy data
pool which is filled with data from the entropy sources.

The CTR-DRBG involves use of AES which is accelerated by the CRYPTO AES
accelerator.

The entropy accumulator of mbed TLS is setup to use SHA-256 (MBEDTLS_ENTROPY_
FORCE_SHA256) which is accelerated by the CRYPTO SHA-256 accelerator.

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The public key is represented in uncompressed point format, which consists of
a DER OCTET STRING tag (0x04) plus the concatenation of the binary
representation of the X coordinate plus the binary representation of the Y 
coordinate of the public point. So the size is equal to two times the field
size (in bytes) + 1. For MBEDTLS_ECP_DP_SECP256R1 ECC curve, the field size is
32 bytes (256 bits) so the public key would be 65 bytes (32 * 2 + 1) long.

To check the performance gain of CRYPTO acceleration, the user can switch off
CRYPTO hardware acceleration by defining NO_CRYPTO_ACCELERATION symbol in IDE
setting.

The example has been instrumented with code to count the number of clock cycles
spent inside the ECDSA API calls, mbedtls_ctr_drbg_seed, mbed TLS_ecdsa_genkey, 
mbed TLS_ecdsa_write_signature and mbed TLS_ecdsa_read_signature.
The results are printed to stdout, i.e. the VCOM serial port console.

The user can change the specific ECC curve used in the example by modifying the
ECPARAMS macro definition. Available curves with CRYPTO acceleration support
are:
MBEDTLS_ECP_DP_SECP256R1 (default)
MBEDTLS_ECP_DP_SECP192R1


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
4. The terminal screen should display the process steps of ECDSA.


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