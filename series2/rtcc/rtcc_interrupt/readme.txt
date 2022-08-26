rtcc_interrupt

This project uses the RTCC (Real Time Clock with Capture) to wake the device
from EM2 (LFRCO or LFXO) or EM3 (ULFRCO) mode.

The wake up interval is defined by WAKEUP_INTERVAL_MS, default value is 500ms.
The RTCC clock source is defined by RTCC_CLOCK, default source is LFXO.

This project also shows how to use the RTCC compare channel PRS output to toggle
LED1 or Expansion header pin 7.

How To Test:
1. Build the project and download it to the Starter Kit
2. Close debug session in IDE
3. Press the reset button on the mainboard
4. Green LED on radio board and yellow LED1 on mainboard flash at 1 Hz

Peripherals Used:
HFRCODPLL - 19 MHz
LFXO - 32768 Hz, RTCC clock source
RTCC - Interrupt every 500ms
PRS - Channel 0, RTCC compare channel PRS output
GPIO

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A Rev. A00) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB01 - Green LED on radio board, yellow LED1 on mainboard

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB00 - Expansion header pin 7
PB01 - Push button PB1

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB00 - Expansion header pin 7, LED0 
PB01 - Push button PB1
