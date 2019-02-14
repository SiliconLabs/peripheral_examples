single_edge_capture

This project demonstrates single edge input capture using the WTIMER module.
WTIMER0 is initialized for input capture on Compare/Capture channel 0 for 
rising edge interrupt. Push button PB0 is configured as the source for PRS 
channel 0, which routes to Compare/Capture channel 0. Upon input capture of 
PB0, the event time is stored in a global variable and LED0 is toggled.

How To Test:
1. Build the project and download to the Starter Kit
2. Press PB0, LED0 should toggle
3. View the captured value written to event_time in the debugger.

Peripherals Used:
WTIMER0 - HFPERclk
PRS	   - GPIO

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
HFRCO  - 19 MHz
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
HFRCO  - 19 MHz
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
HFRCO  - 19 MHz
PF4 - LED0
PF6 - Push Button PB0

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
HFRCO  - 19 MHz
PF4 - LED0
PF6 - Push Button PB0