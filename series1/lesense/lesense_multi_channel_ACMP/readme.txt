Lesense_Multi_Channel_ACMP

This project demonstrates the usage of lesense to sample multiple channels
using ACMP. The project defines four lesense channels and each of these
channels is linked to one of the ACMP channel based on the pin configuration
shown in the datasheet. Each channel will be able to detect a sensor's level
change, and will trigger an interrupt whenever it detects a positive edge
on the sensor's input level.


How to test:
1. Build the project and download it to the starter kit
2. Run the code, connect one of the lesense channel pin to 3.3v
3. Observe LED0 toggle once (on to off or off to on)
4. Disconnect the current channel, and connect a different lesense
   channel pin to 3.3
5. Observe LED0 toggle once again
6. Repeat the process on any of the four lesense channel pins


Peripheral Used:
LFXO - 	32768Hz
ACMP
GPIO
LESENSE


Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC0  - LESENSE Channel 0 (Expansion Header 16)
PC1  - LESENSE Channel 1 (Expansion Header 15)
PC2  - LESENSE Channel 4 (Expansion Header 7)
PC3  - LESENSE Channel 5 (Expansion Header 9)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC8  - LESENSE Channel 0 (Expansion Header 10)
PC9  - LESENSE Channel 1 (Expansion Header 13)
PC10 - LESENSE Channel 2 (Expansion Header 6)
PC11 - LESENSE Channel 3 (Expansion Header 4)


