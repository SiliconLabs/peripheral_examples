i2c_slave

This project demonstrates the slave configuration of the
EFx32xG2x I2C peripheral. Two EFx32xG2x modules are connected together, one
running the master project, the other running the slave project. The master
reads the slave's current buffer values, increments each value, and writes
the new values back to the slave device. The master then reads back the slave
values again and verifies the new values match what was previously written.
This program runs in a continuous loop, entering and exiting EM2 to handle
I2C transmissions. Slave toggles LED0 on during I2C transaction and off when
complete. Slave will set LED1 if an I2C transmission error is encountered.

How To Test:
1.  Connect the SDA, SCL and GND lines between two kits via the EXP header
2.  Jumper 4.7kOhm pull-up resistors from VMCU to the SDA and SCL lines (only one pull-up needed for each line).
3.  Open Simplicity Studio and update each kit's firmware from the Simplicity Launcher (if necessary)
4.  Build both the master and slave projects and download to two Starter Kits
5.  For slave kit, in the drop-down menu, select "Profile As Simplicity Energy Profiler Target"
6.  The project will compile, load and start-up, proceeding to the main loop which sits in EM2
7.  Observe the current consumption in Energy Profiler of the kit in EM2 - typically ~20uA
8.  Toggle push button PB0 on the master kit
9.  Observe the current spike as the slave kit wakes to EM0/1 to handle the I2C transaction and then returns to
    EM2 consumption with each button press of PB0 on the master kit.

Peripherals Used:
FSRCO - 20 MHz
I2C0  - 100 kHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 - LED0, Expansion Header Pin 11, WSTK Pin 8
PB01 - LED1, Expansion Header Pin 13, WSTK Pin 10
PA05 - I2C_SDA, Expansion Header Pin 12, WSTK Pin 9
PA06 - I2C_SCL, Expansion Header Pin 14, WSTK Pin 11

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PD02 - LED0, Expansion Header Pin 11, WSTK Pin 8
PD03 - LED1, Expansion Header Pin 13, WSTK Pin 10
PA05 - I2C_SDA, Expansion Header Pin 12, WSTK Pin 9
PA06 - I2C_SCL, Expansion Header Pin 14, WSTK Pin 11
