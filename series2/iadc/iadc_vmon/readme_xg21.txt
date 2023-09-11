iadc_vmon

 This project demonstrates how to use the window comparator feature in the IADC 
 as a voltage monitor peripheral.
 
 This project uses IADC to monitor AVDD/DVDD 
 Note: AVDD is shorted to VREGVDD and DVDD on the starter kit. So AVDD can be treated
       as VREGVDD/DVDD in this case.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Disconnect Simplicity Studio debugger from the starter kit (just click 
   disconnect button in Simplicity Studio)
4. Place the Starter Kit power switch into BAT to disconnect the debugger's power
5. Attach a power supply to GND and VMCU pins of the WSTK Expansion Header; 
   Positive voltage should be applied to EXP pin 2 and 0.0V applied to EXP pin 1
6. Open a terminal program and connect to the COM port associated with Starter Kit's
   Jlink CDC UART Port (see Windows Device Manager) using 115200 baud, 8-N-1
7.   Press the reset button the mainboard
8. Vary the power supply voltage above/below the thresholds as per the 
   instructions on the terminal program to start testing 
9. Observe the output of testing on the terminal program

Note: When applying voltages independently to supply rails, please observe the 
      operating conditions in the device's data sheet.

================================================================================

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz
EMU
GPIO
IADC    - 12-bit resolution, Automatic 2's Complement (single-ended = unipolar) 
        - Internal VBGR reference (1.21V)
        - IADC and reference kept in warmup mode 
        - Conversions initiated by firmware and triggered continuously
USART   - 115200 baud, 8-N-1

================================================================================

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD02 - Push Button PB0
PD03 - Push Button PB1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1