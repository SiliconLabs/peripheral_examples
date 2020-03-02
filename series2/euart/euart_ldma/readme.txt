euart_dma

This project uses the EUART to receive serial input and then echo it back using
the LDMA. The devices stays in EM2 the entire time since the LDMA module can 
still operate in EM2.

================================================================================
How To Test:
1. Build the project and download it to the Starter Kit.
2. Use a USB to serial device (such as the CP210x). Connect the CP210x's RX pin
   to the device's TX pin. Connect the CP210x's TX pin to the device's RX pin.
   These port and pin mappings are listed below.
3. Open up a serial terminal device such as Termite.
4. In Termite, open the port connected to the CP210x device (check which port
   using Device Manager).
5. Input will be echoed back, after typing in Termite.

================================================================================
Peripherals Used:
LFXO - 32.768 kHz (reference clock for the LFB clock branch)
EUART0 - 9600 baud, 8-N-1 (8 data bits, no parity, one stop bit)
LDMA

================================================================================
Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PA6 - EUART0_RX (Expansion Header Pin 14)
PA5 - EUART0_TX (Expansion Header Pin 12)