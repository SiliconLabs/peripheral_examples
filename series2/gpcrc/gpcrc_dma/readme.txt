gpcrc_dma

This project demonstrates the GPCRC used to check 32-bit data using the 
IEEE 802.3 polynomial standard in EM1. Data is fed into the GPCRC via the LDMA.
The size of the data array being fed into LDMA and GPCRC can be increased 
depending on the application. The example processes 16 words but can be expanded
to process more data as needed. 

Using the GPCRC with LDMA has two benefits:

It does not tie up the CPU. Once the LDMA descriptors are programmed and the 
selected channel is started, the full data array is fed to the GPCRC without any 
CPU intervention.
Execution is deterministic. The LDMA always feeds an entire array into the GPCRC, 
and this is going to have a set execution time (unless other bandwidth intensive 
LDMA traffic is present).

Using the GPCRC peripheral also greatly reduces the software overhead required 
to calculate CRC32. 

The GPCRC can also be used to blank check flash pages. This example (along with
benchmarking data) is available here - https://github.com/SiliconLabs/platform_applications/tree/master/platform_gpcrc_blank_check 

Note for EFR32MG21, clock enabling is not required.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "crcResult" and "FIBONACCI_CRC_16WORDS" 
   to the Expressions window
4. Add a breakpoint at the beginning of crcCheckResult()
5. Run the debugger. You should see it the same values in crcResult and 
   FIBONACCI_CRC_16WORDS.  

================================================================================

Peripherals Used:
HFRCO - 19 MHz
GPCRC - IEEE 802.3 poly standard
LDMA  - Channel 0

================================================================================

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
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