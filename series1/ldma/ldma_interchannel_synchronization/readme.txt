LDMA_Interchannel_Synchronization

This example is based on the Reference Manual Section 7.4.5:
Chapter: LDMA
Section: Examples
Subsection: Example #5


In this example, the LDMA synchronizes across 2 Channels.  Each
channel starts off on a different button press, and transfers 
4-character strings from one software array to another.

If you press PB0, "AAaa" will be transfered to dstBuffer.
If you press PB1, "YYyy" will be transfered to dstBuffer.
After you press the second button (regardless of the order), 
"CCcc" will be transferred to dstBuffer.


How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Run the debugger.  
5) Press either PB0 or PB1, and observe the value in dstBuffer before and
after each button press change according to the behavior described above.

Note: You will have to pause the debugger to see the value in dstBuffer.
