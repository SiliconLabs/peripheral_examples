acmp_vmon

This project demonstrates the usage of the ACMP module in combination with the 
VSENSE inputs to monitor AVDD. VSENSE has a 1/4 voltage divider while VREF uses 
a configurable divider. The VREFDIV divider is configured such that AVDD 
triggers an interrupt when its voltage crosses ~2.54V threshold voltage. 
The ACMP interrupt handler toggles the LEDs on the WSTK and radio board module 
based on AVDD's voltage.  When AVDD is above this threshold, LED1 on the WSTK is 
turned on (along with the green LED on the radio module) and LED0 is turned off. 
When AVDD is below this threshold, LED0 on the WSTK is turned on (along with the 
red LED on the radio module) and LED1 is turned off. 

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Disconnect Simplicity Studio debugger from the starter kit (just click 
   disconnect button in Simplicity Studio)
4. Place the Starter Kit power switch into BAT to disconnect the debugger's power
5. Attach a power supply to GND and VMCU pins of the WSTK Expansion Header; 
   Positive 3.30V should be applied to EXP pin 2 and 0.0V applied to EXP pin 1
6. Vary the power supply voltage above/below the 2.54V threshold (remaining 
   below 3.30V)
7. Observe the LED states on the WSTK and radio board module when above and 
   below the threshold

Note: When applying voltages independently to supply rails, please observe the 
      operating conditions in the device's data sheet.

Note for Testing on xG25:
VMCU only powers IOVDD0-1 and the serial flash on BRD4270B. The 3.6 V LDO powers the
rest of the rails on the radio board. Therefore, in order to use VMCU to control the
threshold for different voltage monitoring rails, VMCU will need to power all the rails.
To test the ACMP voltage monitoring example on BRD4270B, make the following modification:
1. Complete Steps 1 and 2 from the "How to Test" section above.
2. Select the device in the Debug Adapters pane within the Simplicity Studio
   Launcher view. 
2. Select the Documentation tab, then check the Schematic and Layout file 
   resource checkbox. Open the schematic and assembly files for BRD4270B.
3. Unmount resistor R211 and mount resistor R210. Search for the resistors in the assembly 
   file to determine its position on the board.

Peripherals Used:
ACMP    - biasProg value of 0x2; default for capsense
          Reduced input range (0 to VDD-0.7V; this is ok because AVDD is 
                               connected through a 1/4 voltage divider)
          High accuracy mode; 2.5V internal reference; Hysterisis disabled
          VSENSE0 connected to AVDD; VREFDIV set to 0x10 
          -> 2.5V * (16/63) = 0.6349 * 4 (1/4 voltage divider) = 2.54V threshold 
          Rising/falling/ready interrupts enabled

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PB01 - GPIO Push/Pull output, Expansion Header Pin 13, WSTK Pin 10, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 - GPIO Push/Pull output, WSTK Pin 19, LED0
PD03 - GPIO Push/Pull output, WSTK Pin 26, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02 - GPIO Push/Pull output, WSTK Pin 19, LED0
PB04 - GPIO Push/Pull output, WSTK Pin 26, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06 - GPIO Push/Pull output, WSTK Pin 27, LED0
PC07 - GPIO Push/Pull output, WSTK Pin 26, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 - GPIO Push/Pull output, Expansion Header Pin 7, WSTK Pin 4, LED0
PB01 - GPIO Push/Pull output, Expansion Header Pin 9, WSTK Pin 6, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 - GPIO Push/Pull output, WSTK Pin 19, LED0
PD03 - GPIO Push/Pull output, WSTK Pin 23, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 