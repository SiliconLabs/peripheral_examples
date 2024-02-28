gpio_em4_wakeup

This project uses a GPIO pin configured as an input to wake from the EM4
energy mode, thus causing a reset. Pressing the specified button (PB0 for
EFR32MG21 and PB1 for all other devices) on the mainboard wakes the
system from EM4 and toggles the LED0 indefinitely.

Before entering EM4, an "escape hatch" mechanism can be used to pause the
device so that a debugger can connect in order to erase flash, among other
things. This is done by holding pin PC03 low (xG27) or by pressing and
holding button PB1 (xG21) or button PB0 (all other devices).

How To Test:
1. Build the project and download it to the Starter Kit.
2. Terminate the debug session and return to the IDE or Launcher in
   Simplicity Studio,
3. Observe the current consumption of the device to verify that it is in
   EM4 using Studio's Energy Profiler.
4. Press PB0 (EFR32xG21) or PB1 (all other devices) to exit EM4.
5. Observe the LED0 blinking, indicating that the last reset cause was
   exit from EM4. Observing current consumption in Energy Profiler
   verifies that the device is in EM0.

Note for Testing on xG28:
BRD4400C has a RF SPDT switch on the 2.4 GHz radio output that draws
~63 uA of additional current when powered by a logic high level on GPIO
PD02. Unless configured otherwise, this pin defaults to GPIO disabled
mode (high-Z), such that the switch is powered down. Driving PD02 high
turns the switch on, causing current consumption to be higher than what
is specified in the EFR32ZG28 datasheet.

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz
EMU
RMU
USART  - used only to power down the SPI flash on some radio boards

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PB00 -  Red LED on radio board, yellow LED0 on mainboard
PD02 -  Push button PB0
PD03 -  Push button PB1 (Escape hatch)

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22C224F512IM40
PB00 -  Push button PB0 (Escape hatch)
PB01 -  Push button PB1
PD02 -  LED0

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32ZG23B010F512IM48
PB01 -  Push Button PB0 (Escape hatch)
PB03 -  Push Button PB1
PB02 -  LED0

Board:  Silicon Labs EFR32MG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG24B210F1536IM48
PB01 -  Push Button PB0 (Escape hatch)
PB03 -  Push Button PB1
PB02 -  LED0

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
PB00 -  Push Button PB0 (Escape hatch)
PB01 -  Push Button PB1
PC06 -  LED0

Board:  Silicon Labs EFR32xG27 2.4 GHz 8 dBm Buck DCDC Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
PB01 -  Push Button PB1
PB00 -  LED0
PC03 -  Escape Hatch

Board:  Silicon Labs EFR32xG28 868/915 MHz +14 dBm + 2.4 GHz +10 dBm Radio
        Board (BRD4400C) + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  Push Button PB0 (Escape hatch)
PB03 -  Push Button PB1
PB02 -  LED0
