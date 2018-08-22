
================================================================================

This project uses the OFB (Output Feedback Mode) mode of AES encryption to 
encrypt the user's input data and then decrypt it. This project uses 256 bit
keys.

Note: only the series 0 boards have an AES module

================================================================================

Peripherals Used:
HFPERCLK - 14 MHz
AES

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Go into debug mode and click run.
3. View the isError, decryptedData, encryptedData, and 
   originalData global variables
4. If successful, isError will be false

