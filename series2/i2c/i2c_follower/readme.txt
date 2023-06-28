i2c_follower

This project demonstrates the follower configuration of the EFx32xG2x I2C
peripheral. Two EFx32xG2x devices are connected together, one running the leader
project, the other running the follower project. The leader reads the follower's
current buffer values, increments each value, and writes the new values back to
the follower device. The leader then reads back the follower values again and
verifies that the new values match what was previously written. This program
runs in a continuous loop, entering and exiting EM2 to handle I2C transmissions.
Follower toggles LED0 on during I2C transaction and off when complete. Follower
will set LED1 if an I2C transmission error is encountered.

Note: Where possible, the device enters EM2 with:
- RTC enabled with LFRCO as clock source
- DC-DC enabled
- Voltage scale 0
- Full RAM retention

Some of the current consumption values in the datasheet for EM2 are generated
under these test conditions. This example uses the above settings to easily
compare the between the current consumption value described in the datasheet and
the current consumption measured with Simplicity Energy Profiler. These settings
do not affect the functionality of this example and can be removed. Note that
the board's external SPI flash is also powered down as the SPI flash's current
is also measured by the Energy Profiler tool.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

Note: This example uses inclusive lexicon wherever possible. For more
information, visit https://www.silabs.com/about-us/inclusive-lexicon-project

================================================================================

How To Test:
1.  Connect the SDA, SCL and GND lines between two kits via the EXP header
2.  Jumper 4.7kOhm pull-up resistors from VMCU to the SDA and SCL lines (only
    one pull-up needed for each line).
3.  Open Simplicity Studio and update each kit's firmware from the Simplicity
    Launcher (if necessary)
4.  Build both the leader and follower projects and download to two Starter Kits
5.  For follower kit, in the drop-down menu, select "Profile As Simplicity
    Energy Profiler Target"
6.  The project will compile, load and start-up, proceeding to the main loop
    which sits in EM2
7.  Observe the current consumption in Energy Profiler of the kit in EM2
8.  Toggle push button PB0 on the leader kit
9.  Observe the current spike as the follower kit wakes to EM0/1 to handle the
    I2C transaction and then returns to EM2 consumption with each button press
    of PB0 on the leader kit.

================================================================================

Peripherals Used:
HFRCODPLL   - 19 MHz
I2C0        - 100 kHz
RTCC/SYSRTC - 32.768 kHz

================================================================================

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  LED0, Expansion Header Pin 11, WSTK Pin 8
PB01 -  LED1, Expansion Header Pin 13, WSTK Pin 10
PA05 -  I2C_SDA, Expansion Header Pin 12, WSTK Pin 9
PA06 -  I2C_SCL, Expansion Header Pin 14, WSTK Pin 11

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PD02 -  LED0, Expansion Header Pin 11, WSTK Pin 8
PD03 -  LED1, Expansion Header Pin 13, WSTK Pin 10
PA05 -  I2C_SDA, Expansion Header Pin 12, WSTK Pin 9
PA06 -  I2C_SCL, Expansion Header Pin 14, WSTK Pin 11

Board:  Silicon Labs EFR32ZG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB02 -  LED0, WSTK Pin 19
PD03 -  LED1, WSTK Pin 26
PA05 -  I2C_SDA, Expansion Header Pin 7, WSTK Pin 4
PA06 -  I2C_SCL, Expansion Header Pin 11, WSTK Pin 8

Board:  Silicon Labs EFR32MG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB02 -  LED0, WSTK Pin 19
PB04 -  LED1, WSTK Pin 26
PA05 -  I2C_SDA, Expansion Header Pin 7, WSTK Pin 4
PA06 -  I2C_SCL, Expansion Header Pin 11, WSTK Pin 8

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PC06 -  LED0, WSTK Pin 27
PC07 -  LED1, WSTK Pin 26
PA05 -  I2C_SDA, Expansion Header Pin 7, WSTK Pin 4
PA06 -  I2C_SCL, Expansion Header Pin 11, WSTK Pin 8

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 -  LED0, Expansion Header Pin 7, WSTK Pin 4
PB01 -  LED1, Expansion Header Pin 9, WSTK Pin 6
PA05 -  I2C_SDA, Expansion Header Pin 12, WSTK Pin 9
PA06 -  I2C_SCL, Expansion Header Pin 14, WSTK Pin 11

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB02 -  LED0, WSTK Pin 19
PD03 -  LED1, WSTK Pin 23
PA11 -  I2C_SDA, Expansion Header Pin 3, WSTK Pin 0
PA12 -  I2C_SCL, Expansion Header Pin 5, WSTK Pin 2