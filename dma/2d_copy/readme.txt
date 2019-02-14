DMA_2D_Copy

This example is based on the Reference Guide Section 8.4.5:
Chapter: DMA
Section: Functional Description
Subsection: 2D Copy


In this example, the DMA transfers a submatrix from one software matrix 
to another.   The source buffer is numbered from 0 to 
(BUFFER_2D_WIDTH * BUFFER_2D_HEIGHT - 1) in row major order.  Row major 
order was chosen so that the debugger will show the matrix as expected.

The submatrix to be moved will start at (SRC_COL_INDEX, SRC_ROW_INDEX) 
in the source matrix, and be moved to (DST_COL_INDEX, DST_ROW_INDEX) in 
the destination matrix.  The width and height of this submatrix is defined 
by TRANSFER_HEIGHT and TRANSFER_WIDTH.

With the sample values given, the LDMA will transfer

10 11 12
20 21 22
30 31 32
40 41 42

from the source matrix to index (1, 2) of the destination matrix.

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dst2d" to the Expressions window
5) Run the debugger for a second, then pause it
5) You can expand the "dst2d" variable in the Expressions window to see
that the submatrix has been successfully transfered

