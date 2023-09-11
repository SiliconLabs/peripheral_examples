dcdc_vmon

This project demonstrates the DCDC VREGVDD Threshold Comparator.
The device will be switching between DCDC regulator on and DCDC bypass mode 
depending on the input voltage to VREGVDD as below:
- Switch DCDC to bypass mode if VREGVDD is below 2.2 V
- Enable DCDC if VREGVDD is above 2.2 V

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Disconnect Simplicity Studio debugger from the starter kit (just click 
   disconnect button in Simplicity Studio)
4. Place the Starter Kit power switch into BAT to disconnect the debugger's power
5. Attach a power supply to GND and VMCU pins of the WSTK Expansion Header; 
   Positive 3.30V should be applied to EXP pin 2 and 0.0V applied to EXP pin 1
6. Open a terminal program and connect to the COM port associated with Starter Kit's
   Jlink CDC UART Port (see Windows Device Manager) using 115200 baud, 8-N-1
7. Press the reset button the mainboard
8. Vary the power supply voltage above/below the thresholds as per the 
   instructions on the terminal program to start testing 
9. Observe the output on the terminal program

Note: When applying voltages independently to supply rails, please observe the 
      operating conditions in the device's data sheet.
================================================================================

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz
USART  - 115200 baud, 8-N-1
EMU
GPIO

================================================================================

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1