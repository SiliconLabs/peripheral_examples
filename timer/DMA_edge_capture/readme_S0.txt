DMA_edge_capture

This readme applies for Giant Gecko, Wonder Gecko Leopard Gecko, Zero Gecko,
and Happy Gecko

This project demonstrates DMA driven edge capture from a TIMER Compare/Capture
channel. TIMER1 CC0 is configured to capture rising and falling edges. GPIO Pin
PD6 (Expansion Header Pin 16) is to be connected to a periodic signal, and 
edges captured from PD6 are stored in CC0. The LDMA is configured to transfer
the first 512 edges to a fixed length buffer. The buffer is stored globally
for possible future processing.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PD6 (Expansion Header Pin 16)
4. View the buffer[] global array in the debugger

Peripherals Used:
HFRCO    - 14 MHz
TIMER1   - CC0
microDMA - Basic mode

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD6 (Expansion Header Pin 16)  - Input TIM1_CC0 #4