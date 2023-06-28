lesense_multi_channel

This project demonstrates the usage of LESENSE to sample multiple channels
using IADC. The project defines four LESENSE channels and each of these
channels is linked to one of four IADC scan inputs based on IADC configuration
and preset defines at the start of source. Each channel can detect when the 
input sensor conversion data crosses a configured threshold value, and will 
trigger an interrupt on both positive and negative edges of the comparison 
result. Channels 0/2 comparison changes toggle LED0 in the interrupt service
routine while Channels 1/3 comparison changes toggle LED1. The LESENSE alternate
channel configuration is also demonstrated to provide hysteresis for the input
comparison threshold value. The LESENSE configuration uses the LESS threshold
comparison mode, such that the comparison evaluates to 1 if the input sensor
data is less than configured threshold value. 

For push buttons, the discrete onboard logic pulls inputs for channels 0/1 high.
For input channels 2/3 on the external header, please connect these pins through
jumper wire to GND (Expansion Header pins 1 or 19) prior to test procedure below.

Note: In project where the device enters EM2 or lower, an escapeHatch
      routine is usually recommended to prevent device lock-up. This example has
      implemented a escapeHatch, where if the user holds down push button 0 when
      resetting the device, the escapeHatch will hold the device in EM0 state
      to allow debug connection. 

How to test:
1.  Build the project and download it to the starter kit
2.  Run the code, connect LESENSE channel 2 pin (see below) to 3.3V
3.  Observe LED0 toggle once (from ON to OFF)
4.  Connect LESENSE channel 2 pin back to GND
5.  Observe LED0 toggle once (from OFF to ON)
6.  Connect LESENSE channel 3 pin (see below) to 3.3V 
7.  Observe LED1 toggle once (from ON to OFF)
8.  Connect LESENSE channel 3 pin back to GND
9.  Observe LED1 toggle once (from OFF to ON)
10. Press Push Button 0 and hold down
11. Observe LED0 toggle once (from ON to OFF)
12. Release Push Button 0
13. Observe LED0 toggle once (from OFF to ON)
14. Press Push Button 1 and hold down
15. Observe LED1 toggle once (from ON to OFF)
16. Release Push Button 1
17. Observe LED1 toggle once (from OFF to ON)
18. Repeat the process on any of the four LESENSE channel pins

Peripheral Used:
CMU     - LFRCO @ 32768 Hz
        - EM23GRACLK - 1 MHz
IADC    - 12-bit resolution, Unipolar (single-ended), unbuffered 3.3V (AVDD) 
             IADC voltage reference, 2X OSR, IADC and reference running in 
             normal mode; 5 us warmup before each conversion 
        - Conversions initiated by LESENSE; LESENSE channels 0-3 correspond
             to IADC scan table entries 0-3
GPIO
LESENSE - 32 Hz scan rate; LESENSE scans all enabled channels per scan cycle:
             32 samples per second per channel

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  LESENSE Channel 0 (Push Button PB0)
PB03 -  LESENSE Channel 1 (Push Button PB1)
PA06 -  LESENSE Channel 2 (Expansion Header 11)
PA07 -  LESENSE Channel 3 (Expansion Header 13)
PB02 -  LED0
PD03 -  LED1

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  LESENSE Channel 0 (Push Button PB0)
PB01 -  LESENSE Channel 1 (Push Button PB1)
PA06 -  LESENSE Channel 2 (Expansion Header 11)
PA07 -  LESENSE Channel 3 (Expansion Header 13)
PC06 -  LED0
PC07 -  LED1

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  LESENSE Channel 0 (Push Button PB0)
PB03 -  LESENSE Channel 1 (Push Button PB1)
PB04 -  LESENSE Channel 2 (Expansion Header 11)
PB05 -  LESENSE Channel 3 (Expansion Header 13)
PB02 -  LED0
PD03 -  LED1