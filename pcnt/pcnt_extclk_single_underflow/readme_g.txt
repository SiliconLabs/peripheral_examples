PCNT_EXTCLK_SINGLE_UNDERFLOW

For Gecko (EFM32Gxxx):
 This project demonstrates the external clock single input mode of the pulse counter
 using interrupts. BTN0 clock is used as the pulse counter clock in this example. The
 program requests an interrupt whenever the pulse counter goes below zero.
 In this example, each press of Push Button PB0 will decrease the counter value by 1.
 The initial value of the counter and the reload value from the top register is set
 by the user.

Note for Gecko Gxxx devices, the PRS cannot be used with the PCNT

Prepare to Test:
1. Wire PC5 on expansion header(pin 5) to PE0 on the breakout pad
OR connect a function generator to PC5. The frequency setting for
the function generator should be low enough for user to be able to
observe the pulse difference and notice the toggling of the LED.

How To Test:
1. Build the project and download to the Starter Kit
2. Press Button 7 times ( 6 times to sync the clock)
3. Observe LED0 toggle
4. Press Button 0 12 times (equivalent to 6 pulses)
5. Observe LED0 toggle

When the program is first flashed on to the baord, Button 0 needs to be pressed 7 times to
generate enough pulses to sync the external clock. After that there is no
need to press 6 extra times. Note counter is set to zero originally and will
start reloading the top value after the interrupt.

Peripherals Used:
External Clock
PCNT

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PC0 - LED0
PB9 - Push Button PB0
PE0 - PCNT S0


