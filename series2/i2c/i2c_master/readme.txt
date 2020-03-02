i2c_master

This project demonstrates the master configuration of the
EFx32xG2x I2C peripheral. Two EFx32xG2x modules are connected together, one
running the master project, the other running the slave project. The master
starts up and enters a while loop waiting for a button press on push button 0.
When push button 0 is pressed, the program performs an I2C test. This routine
reads the slave device's current buffer values, increments each value by 1,
and writes the new values back to the slave device. The master then reads back
the slave values again and verifies the new values match what was previously
written. Upon a successful write, LED0 is toggled and the device re-enters the
while loop, waiting again for user input through push button 0. This project
runs in a continuous loop, re-running the I2C test with every PB0 button press
and toggling LED0 with each successful iteration. If there is an I2C
transmission error, or if the verification step of the I2C test fails, LED1 is
turned on and the master sits and remains in an infinite while loop. Connecting
to the device via debugger while in the infinite loop, the I2C error code can
be retrieved.

How To Test:
1.  Connect the SDA, SCL and GND lines between two kits via the EXP header
2.  Jumper 4.7kOhm pull-up resistors from VMCU to the SDA and SCL lines (only one pull-up needed for each line).
3.  Open Simplicity Studio and update each kit's firmware from the Simplicity Launcher (if necessary)
4.  Build both the master and slave projects and download to two Starter Kits
5.  On master kit, project will compile, load and start-up, proceeding to the main loop 
6.  Press PB0 on the master kit to trigger an I2C test (read slave buffer, increment, write, read, verify)
7.  Observe LED0 toggle with a successful I2C test
8.  Suspend (pause) the master kit via debugger session
9.  Observe that received values received from the slave (rxBuffer) match transmitted values sent by master (txBuffer)
    by viewing "rxBuffer" and txBuffer respectively in the Expressions window of Simplicity Studio.

Peripherals Used:
FSRCO - 20 MHz
I2C0  - 100 kHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PD02 - Push button PB0, Expansion Header Pin 7, WSTK Pin 4
PB00 - LED0, Expansion Header Pin 11, WSTK Pin 8
PB01 - LED1, Expansion Header Pin 13, WSTK Pin 10
PA05 - I2C_SDA, Expansion Header Pin 12, WSTK Pin 9
PA06 - I2C_SCL, Expansion Header Pin 14, WSTK Pin 11

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB00 - Push button PB0, Expansion Header Pin 7, WSTK Pin 4
PD02 - LED0, Expansion Header Pin 11, WSTK Pin 8
PD03 - LED1, Expansion Header Pin 13, WSTK Pin 10
PA05 - I2C_SDA, Expansion Header Pin 12, WSTK Pin 9
PA06 - I2C_SCL, Expansion Header Pin 14, WSTK Pin 11
