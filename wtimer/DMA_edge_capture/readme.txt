DMA_edge_capture

This project demonstrates DMA driven edge capture from a WTIMER Compare/Capture
channel. WTIMER1 CC0 is configured to capture rising and falling edges. GPIO Pin
PC10 (Expansion Header Pin 16) is to be connected to a periodic signal, and 
edges captured from PC10 are stored in CC0. The LDMA is configured to transfer
the first 512 edges to a fixed length buffer. The buffer is stored globally
for possible future processing.

How To Test:
1. Build the project and download to the Starter Kit
2. Connect a periodic signal to GPIO Pin PD6 (Expansion Header Pin 16)
4. View the buffer[] global array in the debugger

Peripherals Used:
HFRCO    - 19 MHz
WTIMER0   - CC0
LDMA     - Single Byte P2M

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG1P432F1024GL125
PC10 (Expansion Header Pin 16) - Input WTIM0_CC0 #30