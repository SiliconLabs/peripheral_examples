letimer_pulse_width_modulation

This project demonstrates pulse width modulation using the LETIMER.
The project initializes the letimer for PWM with a set 30 percent 
duty cycle at 1 Hz frequency. The waveform is output on an 
Expansion Header Pin on the board.

Note: On EFR32xG21 devices, oscillators and clock branches are automatically 
turned on/off based on demand from the peripherals.  As such, writes to clock 
enable bits are unnecessary and have no effect on xG21 devices. 

Note: On xG22 devices, DEBUG and LETIMER are on the same power 
sub-domain, which means that because LETIMER is enabled in EM2, that power 
sub-domain remains powered in EM2, powering both LETIMER and DEBUG.  Thus, the 
EM2DBGEN bit in EMU_CTRL is not set in the example for xG22. 

Note: On EFR32xG23 devices and later, the DEBUG block on the device is powered 
off by default in low power modes EM2 and below.  The EM2DBGEN bit in EMU_CTRL
can be set to keep DEBUG powered on in EM2, and this has been done in the
example for xG23, xG24, and  xG25, where DEBUG and LETIMER are on different 
power sub-domains. When the EM2DBGEN bit is set, the device will exhibit 
slightly higher EM2 current consumption than when EM2DBGEN is not set.  

How To Test:
1. Build the project and download to the Starter Kit.
2. Measure output on the appriopriate pin listed below.

Peripherals Used:
LETIMER0 - PWM mode


Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA06 -  LET0_O0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA06 -  LET0_O0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PA00 -  LET0_O0 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA00 -  LET0_O0 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PA00 -  LET0_O0 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PA00 -  LET0_O0 (WSTK Pin 21)

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PA00 -  LET0_O0 (WSTK Pin 34)