DMA_Linked_List

This example is based on the Reference Guide Section 8.4.2.3.4:
Chapter: DMA
Section: Functional Description
Subsection: DMA Control
Control Structure: DMA Cycle Types
Cycle Type: Ping-Pong

Software requests a DMA transfer on a linked list of 4 descriptors, 
which each move 8 halfwords of memory into a destination buffer.  The 
callback function is called after every descriptor.

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint inside dmaCallback()
5) Run the debugger.  It should halt inside the callback function with 
the first descriptor complete (this can be seen in the Expressions window)
6) Resume the program.  The debugger should halt again after the next 
descriptor has completed.  Repeat this process until all 6 descriptors
have completed.

