DMA_edge_capture

This readme applies for the Gecko.

This project demonstrates DMA driven edge capture from a TIMER Compare/Capture
channel. TIMER0 CC0 is configured to capture rising and falling edges. GPIO Pin
PD1 (Expansion Header Pin 6) is to be connected to a periodic signal, and 
edges captured from PD1 are stored in CC0. The LDMA is configured to transfer
the first 512 edges to a fixed length buffer. The buffer is stored globally
for possible future processing.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PD1 (Expansion Header Pin 6)
4. View the buffer[] global array in the debugger

Peripherals Used:
HFRCO    - 14 MHz
TIMER1   - CC0
microDMA - Basic mode

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 (Expansion Header Pin 6) - Input TIM0_CC0 #3