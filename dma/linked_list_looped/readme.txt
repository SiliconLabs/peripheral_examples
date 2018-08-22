DMA_Descr_List_With_Looping


In this example, the DMA transfers 4 character strings from one 
software array to another in the pattern:
A, B, A, B, A, B, A, B, C
 -or-
(A, B) 4 times, then C
The transfer is requested by software at the end of the DMA 
initialization.  A Callback function is called after every 
completed transfer.
  

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint inside dmaCallback()
5) Run the program.  The debugger should halt inside the callback 
function with the first descriptor complete (this can be seen in the 
Expressions window)
6) Resume the program.  The debugger should halt inside the callback 
function again, after the next descriptor has completed.  You can do this 
7 more times, then the DMA tranfer will be complete.


