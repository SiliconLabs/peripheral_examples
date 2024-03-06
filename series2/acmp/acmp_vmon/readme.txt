acmp_vmon

This project shows how the ACMP module and its VSENSE inputs can be used
to monitor a supply voltage without CPU intervention.

VSENSE provides access to the AVDD, IOVDD (ACMP0), and DVDD (ACMP1)
supplies that are internally divided-by-4 and connected to the ACMP
positive input when selected. This makes it simple to compare the
specified supply against an arbitrary voltage derived by scaling either
the 1.25 V or 2.5 V internal reference with the VREFDIV divider as the
negative input.

In this example, AVDD is monitored with VSENSE01DIV4 such that the
positive comparator input would range from 1.71 V / 4 = 0.4275 V to
3.8 V / 4 = 0.95 V with the device operating within datasheet-specified
supply limits.

As mentioned above, crossing a desired supply threshold is detected by
using VREFDIV to scale one of the internal references as the negative
comparator input. A 2.54 V threshold is derived here by setting VREFDIV
to 0x10 (16), such that the voltage compared against VSENSE01DIV4 is
2.5 V * (16 / 63) = 0.6349 V. Because the VSENSE interface presents the
supply divided-by-4, VSENSE01DIV4 = AVDD / 4 = 2.54 V / 4 = 0.635 V.

Like the acmp_interrupt and acmp_pin_output examples, operation is in
EM3, such that crossing the threshold in either direction wakes the
device. LED1 is turned on and LED0 is turned off in response to
ACMP_IF_RISE interrupt and vice versa in response to ACMP_IF_FALL.

How To Test:

1. Open Simplicity Studio and update the kit's firmware from the Simplicity Launcher (if necessary).
2. Build the example project and download it to the target system.
3. If in the Simplicity Studio Debugger, disconnect from the target system by terminating the debug sussion (navigate to the Run menu and click Disconnect).
4. Slide the target system power switch to the BAT positrion to disconnect the debugger's power.
5. Attach a power supply to GND and VMCU pins of the mainboard expansion header; 3.30 V should be connected to EXP pin 2 and 0.0 V applied EXP pin 1.
6. Vary the power supply voltage above and below the 2.54 V threshold while otherwise observing the datasheet-specified supply range.
7. Observe the states of LED0 and LED1 on the mainboard when the supply is above and below the threshold.

Simplified Testing With A Pro Kit Mainboard (BRD40002A):

1. Follow steps 1, 2, and 3 above.
2. While in the Simplicity Studio IDE, right-click on the target system in the Debug Adapters panel and select Launch Console...
3. Click the Admin tab, then press Enter.
4. Change the VMCU supply voltage to 2.2 V by typing "target voltage 2.20" (no quotation marks) and pressing Enter. LED0 will be on and LED1 will be off.
5. Raise VMCU back to the typical 3.3 V by typing "target voltage 3.30" (again, no quotation marks) and pressing Enter. LED1 will be on and LED0 will be off.
6. When testing is complete, close the console tab in the IDE.

Testing With xG25:

VMCU powers only the serial flash and the IOVDD0 and IOVDD1 supplies on BRD4270B. A separate 3.6 V LDO on the radio board powers the remaining supply rails. To test supply monitoring on BRD4270B, the LDO must be bypassed so that VMCU can be used instead. This requires two resistor changes that are described below. After making these modifications, testing can be performed as described above.

1. Select the target system in the Debug Adapters panel from within the Simplicity Studio Launcher view.
2. Select the Documentation tab, then check the Schematic and Layout Files Resource Type checkbox. Open the schematic and assembly files for BRD4270B.
3. On the radio board, remove 0 ohm resistor R211 and install 0 ohm resistor R210. Use the assembly drawing to locate the resistors on the radio board.

Reducing Current:

LED0 and LED1 are used in this example for illustrative purposes, but an illuminated LED draws a sizable amount. Naturally, the code that configures the LED GPIO pins and drives them high and low can be removed for the example, especially because a wireless system may communicate battery state to another device over the radio.

To test further current reduction techniques after removing the LED code, consider the following options and use Energy Profiler to observe the impact.

1. Use the duty-cycled VSENSE input to the comparator. In the initACMP() function, change the acmpInputVSENSE01DIV4 parameter in the ACMP_ChannelSet() function call to acmpInputVSENSE01DIV4LP.
2. Use the duty-cycled VREF input to the comparator. In the initACMP() function, change the acmpInputVREFDIV2V5 parameter in the ACMP_ChannelSet() function call to acmpInputVREFDIV2V5LP.
3. Select low-accuracy mode when initializing the comparator.  In the initACMP() function, change acmpAccuracyHigh in the ACMP_Init_TypeDef to acmpAccuracyLow.
4. Use voltage scaling (at the expense of slower CPU wake-up) while in low-energy mode by adding the line "em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;" (no quotation marks) immediately before the EMU_EM23Init(&em23Init) function call.

Keep in mind that these current reduction techniques entail trade-offs
related to system responsiveness and threshold detection accuracy.
Experiment accordingly to determine production system suitability.

Peripherals Used:
ACMP0 - See configuration details above and in the source file.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PB00 - GPIO Push/Pull output, Expansion Header Pin 11, WSTK Pin 8, LED0
PB01 - GPIO Push/Pull output, Expansion Header Pin 13, WSTK Pin 10, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PB02 - GPIO Push/Pull output, WSTK Pin 19, LED0
PD03 - GPIO Push/Pull output, WSTK Pin 26, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32MG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48
PB02 - GPIO Push/Pull output, WSTK Pin 19, LED0
PB04 - GPIO Push/Pull output, WSTK Pin 26, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
PC06 - GPIO Push/Pull output, WSTK Pin 27, LED0
PC07 - GPIO Push/Pull output, WSTK Pin 26, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG27 2.4 GHz 8 dBm Buck DCDC Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
PB00 - GPIO Push/Pull output, Expansion Header Pin 7, WSTK Pin 4, LED0
PB01 - GPIO Push/Pull output, Expansion Header Pin 9, WSTK Pin 6, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio
        Board (BRD4400C) + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 - GPIO Push/Pull output, WSTK Pin 19, LED0
PD03 - GPIO Push/Pull output, WSTK Pin 23, LED1
VMCU - Expansion Header Pin 2
GND  - Expansion Header Pin 1 