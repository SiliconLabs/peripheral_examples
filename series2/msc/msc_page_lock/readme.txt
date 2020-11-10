msc_page_lock

This project demonstrates how to lock flash pages using the
MSC_PAGELOCKn and MSC_MISCLOCKWORD registers.  While the locking
process is a simple matter of writing 1s to the relevant register
bits, positioning of these writes in user program code is somewhat
more complicated.

At first glance, it might seem that the earliest point in program
execution for writes to the lock registers can occur is in main()
immediately after calling CHIP_Init(), which implements any software
fixes for errata workarounds.  However, this is not the case.

Before the first line of main() even executes, the chip has already
run code in the C runtime startup file (e.g. startup_gcc_efr32mg21.s
for EFR32MG21 devices).  For users so inclined, this file can be
modified so that the first instructions executed lock user-specified
flash pages (in addition to any pages that may be locked by the SE on
devices with such functionality).

In following the CMSIS standard, the C runtime startup file for EFR32
devices immediately branches to the SystemInit() function in the
device-specific system settings file (e.g. system_efr32mg21.c for
EFR32MG21 family members).  SystemInit() is provided so that vendors
can initialize system-specific settings or peripherals, and this,
naturally, is the place to locate early, especially write-once,
initialization code, such as locking flash pages.

These example projects lock the last page of main flash in a
customized system file and then attempt to erase it from code running
in main().  Additionally, the system file enables the GPIO pin for
LED0 on the Wireless Starter Kit (WSTK) mainboard and drives it high
to show when the write to MSC_PAGELOCKn takes place.  Once in main(),
the program...

1. calls CHIP_Init(),
2. writes to MSC_MISCLOCKWORD to lock the user data page,
3. drives the LED0 GPIO pin low to show how much time elapses before a
   register write in main() takes place relative to doing so in
   SystemInit(),
4. initializes the MSC for flash write/erase operations,
5. calls MSC_ErasePage() to erase the last page of main flash,
6. halts if this operation returns any status code other than
   mscReturnLocked,
7. calls MSC_ErasePage() to erase the user data page,
8. halts if this operation returns any status code other than
   mscReturnLocked, and
9. loops to itself assuming both pages were locked as expected.

================================================================================

Peripherals Used:

CMU (not required on EFR32xG21)
GPIO
MSC

================================================================================

How To Test:
1. Update the kit's firmware from the Simplicity Launcher, if
   necessary.
2. Build the project and download to the Wireless Starter Kit.
3. Run the program.
4. Press the pause button.  The program will be stopped at the
   while(1) loop at the end of main().
5. To observe how much elapses between issuing critical register
   writes in SystemInit() vs. main(), set an oscilloscope to trigger
   once on a rising edge with a time scale of 25 or 50 µs/division.
6. Observe the LED0 GPIO pin by probing the anode of LED0 on the WSTK
   mainboard (this is the solder pad just below the letter 'E' of the
   'LED0' component label).
7. Run the demo again and observe the pulse on the oscilloscope.

================================================================================

This example is intended to run on the following setup(s):

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG21A010F1024IM32 (this code will run unchanged on the radio
board for any EFR32xG22 variant).

Board: Silicon Labs EFR32xG22 2.4 GHz 6 dBm Board (BRD4182A) 
       + Wireless Starter Kit Mainboard (BRD4001A)

Device: EFR32MG22C224F512 (this code will run unchanged on the radio
board for any EFR32xG22 variant).