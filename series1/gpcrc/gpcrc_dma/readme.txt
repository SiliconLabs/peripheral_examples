GPCRC_LDMA

This project demonstrates the GPCRC used to check 32-bit data using the 
IEEE 802.3 polynomial standard in EM1.  Data is fed into and read out
of the GPCRC via the LDMA.

Functionality is included to show how one could perform this conversion 
without the GPCRC.  Of note, the pure software method requires more memory 
and can take 3-5 times longer to compute.


Note: We use LDMA_DESCRIPTOR_LINKREL_M2M_WORD() here even though we are
performing transfer to/from a peripheral instead of just between memory.
We are able to do this because our transfers are of size 1, so most of the
differences between M2M and M2P transfers end up being irrelevant.  At the 
time of writing this example, there is no LDMA_DESCRIPTOR_XXX that does exatly 
what we want, so we'll have to use one that's close and change the values that 
don't fit our desired use case.


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "result" to the Expressions window
4. Add a breakpoint at the beginning of LDMA_IRQHandler()
5. Run the debugger.  You should see it pause inside the LDMA handler with
a "checked" value inside "result"


Peripherals Used:
HFRCO  - 19 MHz
GPCRC - IEEE 802.3 poly standard
