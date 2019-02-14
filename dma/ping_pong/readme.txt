DMA_Ping_Pong

Software requests DMA ping-pong transfers.  A shared source array is 
transfered first to the "ping" buffer, then to the "pong", then back to ping, 
etc.  After each transfer, there is a callback function that increments the 
elements of the source buffer, then requests the next transfer.  In this way, 
you should see "11111111" transfered to ping, then "22222222" transfered to 
pong, etc.

Note: "ping" is represented by dstBuffer[1], and "pong" is dstBuffer[0]


How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint at the beginning of dmaCallback()
5) Run the debugger.  It should halt inside the interrupt subroutine with 
the first descriptor complete (this can be seen in the Expressions window)
6) Resume the program.  The debugger should halt inside the interrupt 
subroutine again, after the next descriptor has completed. This can be 
repeated any number of times.

