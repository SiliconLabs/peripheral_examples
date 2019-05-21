
================================================================================

This project shows how to use the Cryotimer in EM4 with the ULFRCO and GPIO
retention where the retention is enabled upon entering EM4 and disabled upon 
exiting EM4. This project shows the program execution by using LEDs.

The following is what you should see upon flashing the project to the board:
1. Both LEDs will be off
2. After pushing button 0, LED0 will be on and LED1 will be off
3. After 2 seconds, LED0 will be off and LED1 will be on
4. Go back to step 2

The following is the program flow corresponding to the LEDs:
1. Upon reset, both LEDs will be off. Since the board was reset through powering
   on the board (POR), hitting the reset button (external reset pin), or
   requesting a system reset (through the debugger), LED1 will be turned off.
2. The board will then enter EM3 and wait for the user to push button 0 to
   trigger an interrupt that will wake the board up from EM3. After being woken
   up from EM3, LED0 will be turned on and LED1 will be turned off. Then, the
   Cryotimer will be enabled and the board will enter EM4. However, since GPIO
   retention is turned on, LED0 will still be on in EM4(Note: the current consumption 
   under EM4 mode is still very high due to LED0 on).
3. After two seconds, the Cryotimer will wake us up from EM4 and the reset pin
   will be asserted, causing the program to restart. Since the program was
   configured to disable GPIO retention upon exiting EM4, LED0 will turn off.
   Since the reset cause came from waking up from EM4, LED1 will be turned on.
4. Go back to step 2

Note: the 2 second wait mentioned above is the setting by default and is
dependent on the values of the CRYOTIMER_PERIOD and CRYOTIMER_PRESCALE macros
defined in the source code. These two macros are used together to define the
duration between Cryotimer wakeup events (see the equation below).

The duration between Cryotimer wakeup events is defined to be the following
(in units of seconds):
  Time_Wakeup = (2 ^ PRESC) * (2 ^ PERIODSEL) / (F_CRYOCLK)
  F_CRYOCLK = 1000 Hz for ULFRCO
  F_CRYOCLK = 32768 Hz for LFXO and LFRCO

================================================================================

Note: Since this project goes into EM4, it is important to know how to unlock
the board if it ever gets stuck in EM4 and you can no longer flash the board.

One way to do this is to use the Flash Programmer tool that comes with
Simplicity Studio. Simply click "Erase" before clicking "Program"

Another way to do this is to download Simplicity Commander and then (after
connecting to the board) click "Unlock debug access" under the Flash options.

================================================================================

Peripherals Used:
ULFRCO - 1000 Hz
CRYOTIMER
RMU

================================================================================

How To Test:
0. Build the project and download it to the Starter Kit
1. Both LEDs will be off
2. After pushing button 0, LED0 will be on and LED1 will be off
3. After 2 seconds, LED0 will be off and LED1 will be on
4. Go back to step 2

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PF4 - LED0
PF5 - LED1
PF6 - Push Button 0

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PH10 - LED0 (Red)
PH13 - LED1 (Red)
PC8 - Push Button 0

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - LED0
PC2 - LED1
PD5 - Push Button 0

