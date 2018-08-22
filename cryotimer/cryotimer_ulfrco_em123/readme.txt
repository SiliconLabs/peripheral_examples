cryotimer_ulfrco_em123

This project demonstrates use of the CRYOTIMER with the ultra-low
frequency RC oscillator (ULFRCO) as its clock in EM3.

After chip and DC-DC initialization, the LED0 pin is configured as an
output and driven high, turning on LED0.

The CRYOTIMER is configured to time out after 2K (2048) ULFRCO clock periods,
which is nominally around 2.05 seconds, and request an interrupt.  After this,
the code places the device in EM3 and waits for CRYOTIMER interrupt.

Upon time out, the CRYOTIMER interrupt is requested, waking the device from
EM3.  The interrupt handler clears the CRYOTIMER_IF_PERIOD flag, flushes the
processor core pipeline to make sure the interrupt is not re-requested (the
CRYOTIMER resides in its own, asynchronous clock domains), toggles LED0, and
exits back to the main loop where the sequence repeats.

How To Test:
1. Build the project and download to the Starter Kit
2. LED0 will repeatedly turn on for about 2 seconds (count 1-one thousand,
   2-one thousand to verify) and then turn off for the same amount of time.
3. If desired, observe the change in current draw in Simplicity Studio's
   Energy Profiler.

Note that the program is easily modified to run in either EM1 or EM2 by
replacing the EMU_EnterEM3(false) call in this section...

	// Go into EM3 and let the CRYOTIMER IRQ handler take over
	while(1) EMU_EnterEM3(false);

...with...

	// Go into EM2 and let the CRYOTIMER IRQ handler take over
	while(1) EMU_EnterEM2(false);

...for EM2 or...

	// Go into EM1 and let the CRYOTIMER IRQ handler take over
	while(1) EMU_EnterEM1();

...for EM1.

The code can also be changed to use the low-frequency crystal oscillator (LFXO)
or low-frequency RC oscillator (LFRCO) but must be limited to running in EM1 or
EM2 because these oscillators are disabled in EM3.

Modules Used:
CPU core
GPIO
HFRCO @ 19 MHz (CPU core and GPIO)
ULFRCO
CRYOTIMER

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
Pins Used: PF4 - LED0

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PF4 - LED0

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PF4 - LED0

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PF4 - LED0

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PF4 - LED0