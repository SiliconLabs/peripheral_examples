DMA_Scatter_Gather

For more information on Scatter-Gathers, see Reference Guide Section 8.4.2.3.5:
Chapter: 		LDMA
Section: 		Functional Description
Subsection: 	DMA Control
Topic:			DMA Cycle Types
Cycle Type: 	Memory Scatter-Gather

Software requests a DMA transfer on a linked list of 4 descriptors to "scatter" 
one large array into 4 smaller arrays.  Each descriptor moves 8 halfwords of 
memory into a destination buffer.  An interrupt triggers after the completion 
of the "scatter".

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
4) Run the debugger for a second, then pause it
5) The values in dstBuffer should match the values described above


