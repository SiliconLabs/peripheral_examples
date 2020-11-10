ldma_single_software

In this example, the LDMA transfers 127 halfwords (256 bytes)
from one software array to another.  The transfer is requested
by software.

This example is based on the Reference Manual Section 7.4.1:
Chapter: LDMA
Section: Examples
Subsection: Example #1

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint at the beginning of LDMA_IRQHandler()
5) Run the debugger.  It should halt inside the interrupt subroutine with
the transfer complete (this can be seen in the Expressions window)