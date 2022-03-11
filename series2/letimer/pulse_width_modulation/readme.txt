letimer_pulse_width_modulation

This project demonstrates pulse width modulation using the LETIMER.
The project initializes the letimer for PWM with a set 30 percent 
duty cycle at 1 Hz frequency. The waveform is output on an 
Expansion Header Pin on the board.

Note: On EFR32xG21 devices, oscillators and clock branches are automatically 
turned on/off based on demand from the peripherals.  As such, writes to clock 
enable bits are unnecessary and have no effect on xG21 devices. 

Note: On EFR32xG22 devices and later, the DEBUG block on the device is powered 
off by default in low power modes EM2 and below.  The EM2DBGEN bit in EMU_CTRL
can be set to keep DEBUG powered on in EM2, and this has been done in the
example for xG23, where DEBUG and LETIMER are on different power sub-domains.  
On xG22 devices, DEBUG and LETIMER are on the same power sub-domain, which 
means that because LETIMER is enabled in EM2, that power sub-domain remains
powered in EM2, powering both LETIMER and DEBUG.  Thus, the EM2DBGEN bit in 
EMU_CTRL is not set in the example for xG22.  When the EM2DBGEN bit is set, the 
device will exhibit slightly higher EM2 current consumption than when EM2DBGEN
is not set. 

How To Test:
1. Build the project and download to the Starter Kit
2. Measure output on the appriopriate pin listed below

Peripherals Used:
LETIMER0 - PWM mode


Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - LET0_O0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA6 - LET0_O0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PA0 - LET0_O0 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA0 - LET0_O0 (Expansion Header Pin 5)