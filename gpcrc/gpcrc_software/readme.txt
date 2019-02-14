GPCRC_Software

This project demonstrates the GPCRC used to check an array of 32-bit data
using the IEEE 802.3 polynomial standard. The GPCRC conversions are initiated
by software.

Functionality is included to show how one could perform this conversion 
without the GPCRC.  Of note, the pure software method requires more memory and
can take 3-5 times longer to compute for single words, and the gap grows the more
consecutive transfers you do.


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "results" to the Expressions window
4. Run the debugger, then pause it.  You should notice that "results" is
filled with "checked" values


Peripherals Used:
HFRCO   - 19 MHz
GPCRC   - IEEE 802.3 poly standard
