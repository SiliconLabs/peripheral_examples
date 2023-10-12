i2c_leader_polled

This project demonstrates the leader configuration of the EFx32xG2x I2C
peripheral. Two EFx32xG2x devices are connected together, one running the leader
project, the other running the follower project. The leader starts up and enters
a while loop waiting for a button press on push button 0. When push button 0 is
pressed, the program performs an I2C test. This routine reads the follower
device's current buffer values, increments each value by 1, and writes the new
values back to the follower device. The leader then reads back the follower
values again and verifies the new values match what was previously written. Upon
a successful write, LED1 is toggled and the device re-enters the while loop,
waiting again for user input through push button 0. This project runs in a
continuous loop, re-running the I2C test with every PB0 button press and
toggling LED1 with each successful iteration. If there is an I2C transmission
error, or if the verification step of the I2C test fails, LED1 is turned on and
the leader sits and remains in an infinite while loop. Connecting to the device
via debugger while in the infinite loop, the I2C error code can be retrieved.

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
5.  On leader kit, project will compile, load and start-up, proceeding to the
    main loop 
6.  Press PB0 on the leader kit to trigger an I2C test (read follower buffer,
    increment, write, read, verify)
7.  Observe LED1 toggle with a successful I2C test
8.  Suspend (pause) the leader kit via debugger session
9.  Observe that received values received from the follower (rxBuffer) match
    transmitted values sent by leader (txBuffer) by viewing "rxBuffer" and
    txBuffer respectively in the Expressions window of Simplicity Studio.

================================================================================

Peripherals Used:
HFRCODPLL - 19 MHz
I2C0      - 100 kHz

================================================================================

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 - Push Button PB0, Expansion Header Pin 7, WSTK Pin 4
PB01 - LED1, Expansion Header Pin 9, WSTK Pin 6
PA05 - I2C_SDA, Expansion Header Pin 12, WSTK Pin 9
PA06 - I2C_SCL, Expansion Header Pin 14, WSTK Pin 11