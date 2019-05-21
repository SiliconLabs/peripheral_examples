DMA_Single_Software

In this example, the DMA transfers 128 halfwords (256 bytes) from one 
software array to another.  The transfer is requested by software, and
a callback function is called upon completion.

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint inside dmaCallback()
5) Run the debugger.  The debugger should halt inside the callback function 
with the transfer complete (this can be seen in the Expressions window)
