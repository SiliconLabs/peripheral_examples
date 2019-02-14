EMU_Voltage_Scaler

This project demonstrates the Voltage Scaling capabilities of the EMU.  When PB0
is pressed, the handler switches between VSCALE0 and VSCALE2.  Using the energy
profiler, you can then observe the differences is current draw between these two
voltage scaling values.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Energy Profiler
4. Observe the measured current draw in the expressions window and how it responds
to button presses (pressing PB0 should switch between VSCALE 0 and VSCALE 2)

Peripherals Used:
HFRCO - 19 MHz
EMU -   VSCALE


Below is Sample Data observed with our STKs:

STK   | VS2(mA)   VS0(mA)   % Decrease
--------------------------------------
GG11  | 3.15      2.19      30.5
TG11  | 1.59      1.34      15.7
PG12  | 2.67      2.24      16.1
      |
BG12  | 2.52      2.11      16.3
BG13  | 2.21      1.86      15.8
BG14  | 2.17      1.83      15.7
      |
FG12  | 2.52      2.11      16.3
FG13  | 2.30      1.92      16.5
FG14  | 2.17      1.81      16.6
      |
MG12  | 2.58      2.16      16.3
MG13  | 2.27      1.90      16.3
MG14  | 2.16      1.81      16.2

