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

Note for Testing on xG25:
VMCU only powers IOVDD0-1 and the serial flash on BRD4270B. The 3.6 V LDO powers the
rest of the rails on the radio board. Therefore, in order to use VMCU to control the
threshold for different voltage monitoring rails, VMCU will need to power all the rails.
To test the DC-DC voltage monitoring example on BRD4270B, make the following modification:
1. Complete Steps 1 and 2 from the "How to Test" section above.
2. Select the device in the Debug Adapters pane within the Simplicity Studio
   Launcher view. 
2. Select the Documentation tab, then check the Schematic and Layout file 
   resource checkbox. Open the schematic and assembly files for BRD4270B.
3. Unmount resistor R211 and mount resistor R210. Search for the resistors in the assembly 
   file to determine its position on the board.

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

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 