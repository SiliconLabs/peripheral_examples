LDMA_Scatter_Gather

This example is based on the Reference Manual Section 7.4.8:
Chapter: LDMA
Section: Examples
Subsection: Example #8

Software requests an LDMA transfer on a linked list of 4 descriptors to
"scatter" one large array into 4 smaller arrays.  Each descriptor moves 8 
halfwords of memory into a destination buffer.  An interrupt triggers after 
the completion of the "scatter".

The scattering is done out of order so that the functional difference between 
this and a single descriptor are more clear.  The source array contains 
values from 0 to (BUFFER_SIZE * 4 - 1), and they get transfered to arrays of 
size BUFFER_SIZE starting with dstBuffer[3], then dstBuffer[2], etc.  
Therefore, the result dstBuffer matrix will look like:

dstBuffer[0] = 24, 25, 26, 27, 28, 29, 30, 31
dstBuffer[1] = 16, 17, 18, 19, 20, 21, 22, 23
dstBuffer[2] =  8,  9, 10, 11, 12, 13, 14, 15
dstBuffer[3] =  0,  1,  2,  3,  4,  5,  6,  7


How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint inside LDMA_IRQHandler()
5) Run the debugger. It should halt inside the interrupt handler with values 
in dstBuffer that match the values described above


