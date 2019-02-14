LDMA_2D_Copy

This example is based on the Reference Manual Section 7.4.6:
Chapter: LDMA
Section: Examples
Subsection: Example #6


In this example, the LDMA transfers a submatrix from one software 
matrix to another.   The source buffer is numbered from 0 to 
(BUFFER_2D_WIDTH * BUFFER_2D_HEIGHT - 1) in row major order.  Row major 
order was chosen so that the debugger will show the matrix as expected.

The submatrix moved will start at (SRC_COL_INDEX, SRC_ROW_INDEX)
in the source matrix, and be moved to (DST_COL_INDEX, DST_ROW_INDEX)
in the destination matrix.  The width and height of this submatrix 
is defined by TRANSFER_HEIGHT and TRANSFER_WIDTH.

With the sample values given, the LDMA will transfer

10 11 12
20 21 22
30 31 32
40 41 42

from the source matrix to index (2, 1) of the destination matrix.

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dst2d" to the Expressions window
4) Add a breakpoint to the beginning of LDMA_IRQHandler()
5) Run the debugger.  It should halt inside the interrupt handler
5) You can expand the "dst2d" variable in the Expressions window to see
that the submatrix has been successfully transfered

