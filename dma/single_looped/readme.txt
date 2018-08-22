DMA_Single_Looped_Transfer

In this example, the DMA transfers 8 halfwords from one software 
array to another.  It will loop this process an infinite number
of times.  The transfer is requested by software at the end of 
the DMA initialization and again after each callback function.

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint inside dmaCallback()
5) Run the debugger.  It should halt inside the callback function with 
the first descriptor complete (this can be seen in the Expressions window)
6) Resume the program.  The debugger should halt inside the interrupt 
subroutine again, after the next descriptor has completed.  You can do
this an infinite number of times.

