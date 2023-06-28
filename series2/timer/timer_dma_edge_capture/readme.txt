timer_dma_edge_capture

This project demonstrates DMA driven edge capture from a TIMER Compare/Capture 
channel. TIMER0 CC0 is configured to capture rising and falling edges. A GPIO 
Pin is to be connected to a periodic signal, and edges captured from GPIO pin 
are stored in CC0. The LDMA is configured to transfer the first 512 edges to a 
fixed length buffer. The buffer is stored globally for possible future 
processing.

Note: For EFR32xG21 radio devices, library function calls to CMU_ClockEnable() 
have no effect as oscillators are automatically turned on/off based on demand 
from the peripherals; CMU_ClockEnable() is a dummy function for EFR32xG21 for 
library consistency/compatibility.

================================================================================

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin (see board specific pin below)
3. View the buffer[] global array in the debugger

================================================================================

Peripherals Used:
CMU    - HFRCO @ 19 MHz
TIMER0 - EM01GRPACLK
LDMA

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA06  - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A)
        + Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA06  - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32ZG23 868-915 MHz 14 dBm Radio Board (BRD4204D)
        + Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32xG24 2.4 GHz 10 dBm Radio Board (BRD4186C)
        + Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32FG25 902-928 MHz 14 dBm Radio Board (BRD4270B)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32FG25B222F1920IM56
PA06  - TIM0_CC0 (Expansion Header Pin 11)

Board:  Silicon Labs EFR32MG27 Radio Board (BRD4194A)
        + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG27C140F768IM40
PA06  - TIM0_CC0 (WSTK P11, Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C)
        + Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB04  - TIM0_CC0 (WSTK P8, Expansion Header Pin 11)