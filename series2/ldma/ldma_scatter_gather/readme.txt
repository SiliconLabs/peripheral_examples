ldma_scatter_gather

This example is based on the EFR32 Series 2 Reference Manual:
Chapter: LDMA
Section: Examples
Subsection: Example #8

Software requests a LDMA transfer on a linked list of 4 descriptors to "scatter"
one large array into 4 smaller arrays. Each descriptor moves 8 halfwords of
memory into a destination buffer. An interrupt triggers after the completion
of the "scatter".

The scattering is done out of order so that the functional difference between 
this and a single descriptor are more clear. The source array (srcBuffer)
contains values from 0 to (BUFFER_SIZE * 4 - 1), and they get transfered to 
arrays of size BUFFER_SIZE starting with dstBuffer[3], then dstBuffer[2], etc.  

Therefore, the result dstBuffer matrix will look like:
dstBuffer[0] = 24, 25, 26, 27, 28, 29, 30, 31
dstBuffer[1] = 16, 17, 18, 19, 20, 21, 22, 23
dstBuffer[2] =  8,  9, 10, 11, 12, 13, 14, 15
dstBuffer[3] =  0,  1,  2,  3,  4,  5,  6,  7

Then software requests a LDMA transfer on a linked list of 4 descriptors to
"gather" 4 smaller arrays into one large array. Each descriptor moves 8
halfwords of memory into one destination buffer. An interrupt triggers after
the completion of the "gather".

The gathering is done out of order so that the functional difference between 
this and a single descriptor are more clear.  The dstBuffer[n] becomes the 
source arrays and they get transfered to srcBuffer starting with dstBuffer[3],
then dstBuffer[2], etc.

Therefore, the result srcBuffer will look like:
srcBuffer =  0,  1,  2,  3,  4,  5,  6,  7,
          =  8,  9, 10, 11, 12, 13, 14, 15,
          = 16, 17, 18, 19, 20, 21, 22, 23,
          = 24, 25, 26, 27, 28, 29, 30, 31

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open Simplicity Debugger and add "srcBuffer" and "dstBuffer" to the
   Expressions window
4. Add a breakpoint inside LDMA_IRQHandler()
5. Run the debugger. It should halt inside the interrupt handler with values in
   dstBuffer that match the values described above
6. Add a breakpoint below inside initLdmaGather()
   LDMA_TransferCfg_t memTransferTx = LDMA_TRANSFER_CFG_MEMORY();
7. Run the debugger. It should halt inside initLdmaGather() and srcBuffer is
   clear to 0
8. Run the debugger. It should halt inside the interrupt handler again with
   values in srcBuffer that match the values described above

================================================================================

Peripherals Used:
HFRCODPLL - 19 MHz
LDMA      - Channel 0

================================================================================

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21A010F1024IM32

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40

Board:  Silicon Labs EFR32ZG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68