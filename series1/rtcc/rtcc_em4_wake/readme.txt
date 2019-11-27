This project demonstrates use of the RTCC to wake from EM4.

Once the device is initialized, it toggles LED0 on or off depending on
its last state, enters EM4 for 5 seconds, and then wakes up.

Because it is not possible to connect to a device in EM4, there is also
an "escape hatch" mechanism available  Hold down PB0, and upon wake
from EM4, the device will turn on LED0 and LED1 and execute the BKPT
instruction.  This stops the processor while it is still in EM0, thus
allowing a debugger to make a connection.

================================================================================

Peripherals Used:

CMU
GPIO
RMU
RTCC
USART1

================================================================================

How To Test:
1. Build the project and download to the Starter Kit (STK).
2. Click the Run/Resume button in the debugger.  Note that the
   debugger will immediately lose its connection to the STK because
   it is not possible to maintain a debug connection in EM4.  This is
   expected.
3. To stop the processor in EM0, hold down PB0 until both LED0 and LED1
   turn on.  At this point, it is now possible to connect to the STK
   in order to erase the flash and prevent the example code from
   running.  This can be done with the flash programmer in Simplicity
   Studio or with Simplicity Commander.

================================================================================

NOTES:

1. All of the examples make use of the Gecko Software Development Kit
Board Support Package (BSP) to access the pins used for button 0 (PB0),
LED0, and LED1.  Note that the RGB LEDs on the GG11 and GG12 kits are
illuminated driving the BSP-specified port pins low instead of high as
is the case for all of the other device kits.  Refer to the kit
documentation to determine the specific pins associated with the LEDs
and push buttons.

2. Certain radio stacks (e.g. BLE) for EFR32 devices use the RTCC for
timing purposes.  For this reason, it is not generally possible to use
the RTCC in EM0/EM1/EM2 with these stacks.  However, because wake from
EM4 is a reset and thus requires restarting the device (including the
radio), using the RTCC to implement a timed wake should not present
any problems.  Refer to the documentation for the radio stack being
used for further information.