LDMA_Linked_List

This example is based on the Reference Manual Section 7.4.2:
Chapter: LDMA
Section: Examples
Subsection: Example #2

Software requests an LDMA transfer on a linked list of 4 descriptors, 
which each move 32 halfwords of memory into a destination buffer.  
There are interrupts triggered after the second and fourth descriptors.

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint at the beginning of LDMA_IRQHandler()
5) Run the debugger.  It should halt inside the interrupt subroutine with
the first two descriptors complete (this can be seen in the Expressions 
window)
6) Resume the debugger.  It should halt inside the interrupt subroutine 
again after all the descriptors have completed.


