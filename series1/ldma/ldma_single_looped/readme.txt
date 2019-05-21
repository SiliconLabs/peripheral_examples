LDMA_Single_Looped

This example is based on the Reference Manual Section 7.4.3:
Chapter: LDMA
Section: Examples
Subsection: Example #3


In this example, the LDMA transfers 16 words from one software 
array to another in 4 sets of 4 words.  The transfer is requested 
by software at the end of the LDMA initialization.

If you wanted to have the transfers run automatically without
interrupts, you can remove from initLdma() the following lines:

  descLink.xfer.doneIfs     = true;                     // Enable interrupts
  descLink.xfer.structReq   = false;                    // Disable auto-requests
	
	... (Do not remove the lines between these)
	
	// Send software request
	LDMA->SWREQ |= LDMA_CH_MASK;
  
Also remove from LDMA_IRQHandler() the following lines:

  // Start next Transfer
  LDMA->SWREQ |= LDMA_CH_MASK;


How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint at the beginning of LDMA_IRQHandler()
5) Run the debugger.  It should halt inside the interrupt subroutine with
the first descriptor complete (this can be seen in the Expressions window)
6) Resume the program.  The debugger should halt inside the interrupt 
subroutine again, after the next descriptor has completed.  You can do
this 2 more times, then the LDMA tranfer will be complete.

