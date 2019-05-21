DMA_Single_Button


In this example, the DMA transfers 128 halfwords (256 bytes) from one 
software array to another.  The transfer is requested by a button press 
on Push Button 1 routed through the PRS.  A Callback Function is called
when the tranfer is complete.

Because of the limitations of Series 0, button presses cannot directly
request DMA transfers.  In order to acheive this funcitonality, TIMER0 
Input Capture had to be introduced.  Button presses triggers input 
captures, and the CC0 (Capture Complete) flag in turn requests the DMA
transfer.

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint inside dmaCallback()
5) Run the debugger.  After you press the button, the debugger should halt 
inside the callback function with the transfer complete (this can be seen 
in the Expressions window)
