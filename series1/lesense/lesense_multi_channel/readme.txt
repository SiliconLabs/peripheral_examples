Lesense_Multi_Channel

This project demonstrates the usage of lesense to sample multiple channels using ACMP. The project
defines four lesense channels and each of these channels is linked to one of the ACMP channel based
on the datasheet. The project is tested on the Tiny Gecko 11 and Giant Gecko 11 board. For Tiny
Gecko 11 all pins will be avaliable on the expansion header, for Giant Gecko 11 only two of the
pins will be avaliable, a socket board will be required for all four pins.


How to test:
1. Build the project and download it to the starter kit
2. Run the code, connect one of the lesense channel pin to 3.3v
3. Observe LED0 toggle once (on to off or off to on)
4. Disconnect the current channel, and connect a different lesense
   channel pin to 3.3
5. Observe LED0 toggle once again
6. Repeat the process on any of the four lesense channel


Peripheral Used:
ACMP
GPIO
LFXO - 	32768HZ
HFLE
LESENSE


Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC0  - LESENSE Channel 0
PC1  - LESENSE Channel 1
PC2  - LESENSE Channel 2
PC3  - LESENSE Channel 3

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC8  - LESENSE Channel 0
PC9  - LESENSE Channel 1
PC10 - LESENSE Channel 2
PC11 - LESENSE Channel 3


