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