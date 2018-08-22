I2S

This project demonstrates how to get raw microphone data using i2s and
transferring it over USART via LDMA transfers. Only the left microphone  
channel is used while the right microphone channel is just read into a dummy variable.
This data can then be used for different purposes such as spectral analysis. 
See the GG11 spectral analysis example for an example showing what the data can be used for.

How To Test:
1. Build the project(s) and download to GG11
2. Watch the leftBuffer and periodically pause
execution to view the contents of it from the microphone

Peripherals Used:
I2S - 34133 Hz

Board:  Silicon Labs EFR32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
I2S TX  -  12
I2S RX  -  13
I2S CS  -  15
I2S CLK -  14