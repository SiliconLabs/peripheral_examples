iadc_vmon

 This project demonstrates how to use the window comparator feature in the IADC 
 as a voltage monitor peripheral. 
 
 The IADC is enabled in continous single conversion mode with the internal 1.2 V 
 bandgap voltage as its reference voltage. This  project demonstrates supply 
 voltage movitoring of the following four modes:
 1. Monitor AVDD with DCDC set to Bypass
 2. Monitor AVDD with DCDC enabled
 3. Monitor DVDD with DCDC set to Bypass
 4. Monitor DVDD with DCDC enabled
 
 Note: AVDD is shorted to VREGVDD on the starter kit. So AVDD can be treated
       as VREGVDD in this case.
 
When DCDC is enabled, VREGVDD(AVDD in this case) should have its voltage
between 2.2-3.8V, the example uses those two values as the threshold.

When DCDC is bypassed, AVDD should be in range of 1.8 to 3.8V,
this example uses 1.9 and 3.7 as the threshold to give early warnings.

When DCDC is enabled, DVDD should be around 1.8V,
this example uses 1.755 and 1.86 as threshold.

When DCDC is bypassed, DVDD should be the same as AVDD,
this example still uses 1.9-3.7V as the threshold.

The threshold for bypass mode is chosen as a early warning voltage level. The 
DVDD rail with DCDC enabled should be around 1.8 V; the AVDD rail is shorted to 
VREGVDD on the STK, therefore it should follow the VREGVDD voltage requirement 
when DCDC is enabled, which should be a programmable value between 2.0-2.3 V.

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
7. Press the reset button the mainboard
8. Follow instructions in the terminal program. Use Push Button 0 (PB0) to 
   navigate through different testing mode, and press Push Button 1 (PB1) to 
   start testing
9. Test different modes with various supply voltage to trigger different 
   comparison conditions

Note: When applying voltages independently to supply rails, please observe the 
      operating conditions in the device's data sheet.
      
================================================================================

Note for Testing on xG25:
VMCU only powers IOVDD0-1 and the serial flash on BRD4270B. The 3.6 V LDO powers the
rest of the rails on the radio board. Therefore, in order to use VMCU to control the
threshold for different voltage monitoring rails, VMCU will need to power all the rails.
To test the IADC voltage monitoring example on BRD4270B, make the following modification:
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
EMU
GPIO
IADC    - 12-bit resolution, Automatic 2's Complement (single-ended = unipolar) 
        - Internal VBGR reference (1.21V)
        - IADC and reference kept in warmup mode 
        - Conversions initiated by firmware and triggered continuously
USART   - 115200 baud, 8-N-1

================================================================================

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22A224F512IM40
PB00 -  Push Button PB0
PB01 -  Push Button PB1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  Push Button PB0
PB03 -  Push Button PB1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  Push Button PB0
PB03 -  Push Button PB1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  Push Button PB0
PB01 -  Push Button PB1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 -  Push Button PB0
PB01 -  Push Button PB1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  Push Button PB0
PB03 -  Push Button PB1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1