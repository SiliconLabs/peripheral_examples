LDMA_Ping_Pong

This example is based on the Reference Manual Section 7.4.7:
Chapter: LDMA
Section: Examples
Subsection: Example #7

Software requests LDMA ping-pong transfers.  A universal source array is 
transfered first to the "pingBuffer" array, then to the "pongBuffer", then
back to ping, etc.  After each transfer, there is an interrupt that
increments the elements of the source buffer, then requests the next transfer.  
In this way, you should see "11111111" transfered to ping, then "22222222" 
transfered to pong, etc.


How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "pingBuffer" and "pongBuffer" to the 
Expressions window
4) Add a breakpoint at the beginning of LDMA_IRQHandler()
5) Run the debugger.  It should halt inside the interrupt subroutine with 
the first descriptor complete (this can be seen in the Expressions window)
6) Resume the program.  The debugger should halt inside the interrupt 
subroutine again, after the next descriptor has completed.


