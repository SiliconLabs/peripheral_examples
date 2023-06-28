ldma_linked_list_looped

This example is based on the EFR32 Series 2 Reference Manual:
Chapter: LDMA
Section: Examples
Subsection: Example #4

In this example, the LDMA transfers 4 character strings from one software
array to another. It switches between A and B four times, then transfers C,
like so:
A, B, A, B, A, B, A, B, C

The transfer is requested by software at the end of the LDMA initialization.

If you wanted to have the transfers run automatically without interrupts, 
you can remove from initLdma() the following lines:

	// Enable interrupts
	descLink[0].xfer.doneIfs = true;
	descLink[1].xfer.doneIfs = true;
	descLink[2].xfer.doneIfs = true;

	// Disable automatic triggers
	descLink[0].xfer.structReq = false;
	descLink[1].xfer.structReq = false;
	descLink[2].xfer.structReq = false;
	
	... (Do not remove the lines between these)
	
	// Request first transfer
	LDMA->SWREQ |= LDMA_CH_MASK;
  
Also remove from LDMA_IRQHandler() the following lines:

	// Request next transfer
	LDMA->SWREQ |= LDMA_CH_MASK;

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4. Add a breakpoint at the beginning of LDMA_IRQHandler()
5. Run the debugger. It should halt inside the interrupt subroutine with the
   first descriptor complete (this can be seen in the Expressions window)
6. Resume the program. The debugger should halt inside the interrupt subroutine
   again, after the next descriptor has completed. You can do this 7 more times,
   then the LDMA tranfer will be completed.

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