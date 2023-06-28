em23_voltage_scaling

This project demonstrates the use of EM2/EM3 voltage scaling on Series 2
EFR32 devices.  Note that this differs from the voltage scaling
functionality on Series 1 devices where a single setting (VS2 or VS0)
applies in all operating modes.  For Series 2 devices, separate examples
are provided for EM0/1 and EM2/3 voltage scaling.

In this example, the CPU (a) configures the BURTC to request interrupts
at 4-second intervals, (b) sets the RAM to power-down all blocks when
entering EM2/3 except for the RAM block 0 holding the stack, and
(c) enters EM3 with VS2 voltage scaling.  Upon wake-up, EM23 voltage
scaling is switched to VS0, and the device re-enters EM3.  Toggling
between VS2 and VS0 happens every four seconds so that it can observed
in Simplicity Studio's Energy Profiler or by powering the WSTK from a
bench supply with a digital multimeter in series and sliding the
3-position BAT/USB/AEM switch from the AEM to the BAT position.

The savings when running at VS0 (0.9 V) vs. VS2 (1.1 V) in EM3 with the
ULFRCO providing the clock for the BURTC is on the order of 40% (1.45 uA
vs. 2.52 uA) when this example is run on EFR32MG22 revision C silicon
and observed using using Energy Profiler.

When EFR32xG22 and later devices enter EM2/3, the clock to the CPU debug
block is stopped, effectively dropping the host OC debugger connection.
This examples includes an "escape hatch" mechanism to pause the device so
that a debugger can connect in order to erase flash.  To use this, press
the PB0 push button and, while holding the button, press the RESET button
in the lower right corner of the WSTK.  Upon releasing the RESET button,
the device runs code that sees that PB0 is depressed, turns on LED0, and
issues a breakpoint instruction to halt the CPU.  At this point, a
the debugger connection can be resumed to erase flash, etc.

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Wireless Starter Kit, then exit
   from the debugger.
3. Open the Simplicity Studio's Energy Profiler.  Select Start Energy Capture
   from the Profiler menu.
4. Zoom in on the Y-axis (current) and observe the change in current draw
   as the devices toggles between VS2 and VS0 every four seconds.

================================================================================

Note for Testing on xG25:
Simplicity Studio's Energy Profiler tool only measures the VMCU current. VMCU 
only powers IOVDD0-1 and the serial flash on BRD4270B. The 3.6 V LDO powers the
rest of the rails on the radio board. In the datasheet, the current 
consumption with DCDC test conditions requires IOVDD0-2, RFVDD, and DVDD to be 
powered by the output of the DCDC. Keep in mind, due to the design of the radio
board (BRD4270B), this example does not replicate the datasheet test conditions
followed for current consumption with DCDC. Although RFVDD and DVDD are powered 
by the output of the DCDC, IOVDD0-1 is powered by VMCU on the radio board and 
IOVDD2 is powered by the USB_VREG. To measure the 3.6V LDO current, the 
following extra steps must be taken.
1. Complete Steps 1 and 2 from the "How to Test" section above.
2. Select the device in the Debug Adapters pane within the Simplicity Studio
   Launcher view. 
2. Select the Documentation tab, then check the Schematic and Layout file 
   resource checkbox. Open the schematic and assembly files for BRD4270B.
3. Resistor R247 is a 0 Ohm resistor that can be removed to  measure the 
   current consumption of the device. Search for this resistor in the assembly 
   file to determine its position on the board. This resistor is present on the 
   underside of BRD4270B. 
4. Remove R247 and solder two leads to each pad where R247 was previously. 
   Connect these leads to a multimeter to measure the current consumption of the 
   device.

Note for Testing on xG27:
On BRD4194A, VMCU is a 3.3V supply that powers AVDD and IOVDD on xG27.
In the datasheet, current consumption test conditions have AVDD and IOVDD
powered by either the DC-DC at 1.8V, an external 1.8V supply, 
or an external 3.0V supply. Due to the design of the radio board, this 
board does not replicate the datasheet test conditions for current consumption,
and the measured value may differ from the datasheet value.
On BRD4111A, the VMCU is the input to an on-board voltage regulator that outputs
1.5V, which is then connected to the boost DC-DC input, VBAT. This additional
voltage regulator consumes an extra 10 to 12 uA of current and will make the
energy profiler current measurement higher than the datasheet specification. 
Running this example on an out-of-box BRD4111A + BRD4001A WSTK will result in
a current reading of ~16uA when using the energy profiler. 
To get a more accurate EM2 current consumption reading of the device,
the following extra steps must be taken:
1. Flash the hex image onto the device by following the "How to Test" section
2. On the BRD4111A board, make the following modification:
  a. Unmount 0 ohm resistor R222, mount 0 ohm resistor R223
  b. unmount 0 ohm resistor R211, mount 0 ohm resistor R210
  c. In main_xg27_boost.c, set MX25_POWER_DOWN definition to (0)
3. Steps a and b are required to use the boost DC-DC output to supply all rails
   Step c is needed to eliminate current leakage caused by the unpowered
   MX25_POWER_DOWN SPI flash.
4. Place a header on ST1 (bottom left of the front of the radio board).
5. Using an external power supply to supply 1.5V via the headers.
6. Measure the current using a bench meter from the 1.5V supply to
   the pre-programmed, standalone BRD4111A radio board.
   
Note for Testing on xG28:
On BRD4400C, there is a diversity SPDT switch on the 2.4 GHz radio output that
will consume ~63 uA of additional current when supplied power via logic high on
GPIO PD02. The pin is configured in disabled mode (high-Z) by default, thus SPDT
switch is powered down by default. Be advised that when PD02 is driven to logic
high, observed current consumption will be higher than what is specified in the 
EFR32ZG28 device datasheet due to this additional integrated circuit.

================================================================================

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz, ULFRCO @ 1 kHz
BURTC  - 1 kHz ULFRCO clock source
USART0 - 8 Mbits/s 
EUSART - 8 Mbits/s (xG25)

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22C224F512
PB00  - push button PB0
PD02  - LED0
PC00  - USART0 MOSI
PC01  - USART0 MISO
PC02  - USART0 SCLK
PA04  - USART0 CS

Board: Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Board (BRD4204D)
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PB01  - push button PB0
PB02  - LED0
PC01  - USART0 MOSI
PC02  - USART0 MISO
PC03  - USART0 SCLK
PC04  - USART0 CS

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01  - push button PB0
PB02  - LED0
PC01  - USART0 MOSI
PC02  - USART0 MISO
PC03  - USART0 SCLK
PC04  - USART0 CS

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00  - push button PB0
PC06  - LED0
PC00  - FLASH MOSI
PC01  - FLASH MISO
PC02  - FLASH SCLK
PC03  - FLASH CS

Board:  Silicon Labs EFR32xG27 Buck Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00  - push button PB0
PB00  - LED0
PC00  - FLASH MOSI
PC01  - FLASH MISO
PC02  - FLASH SCLK
PA04  - FLASH CS

Board:  Silicon Labs EFR32xG27 Boost Radio Board (BRD4111A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG27C320F768GJ39
PC05  - push button PB0
PC05  - LED0
PB00  - FLASH MOSI
PB01  - FLASH MISO
PB02  - FLASH SCLK
PC02  - FLASH CS

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01  - push button PB0
PB02  - LED0
PC01  - FLASH MOSI
PC02  - FLASH MISO
PC03  - FLASH SCLK
PC04  - FLASH CS