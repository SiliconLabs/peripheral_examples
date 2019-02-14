
================================================================================

This example uses the LEUART to receive serial input and then echo it back using
the DMA/LDMA (Linked DMA is for series 1 devices only). The devices stays in EM2
the entire time since the DMA/LDMA module can still operate in EM2.

Note: The Zero Gecko, Happy Gecko, Gecko, and Tiny Gecko devices will not stay in
EM2 the entire time. This is because the device will be woken up to service the
DMA interrupt handler that refreshes the DMA transfer cycle. This is necessary
because the devices listed above do not support DMA looping.

This example used about 10.8 microamps on average (this spikes to about 67
microamps when sending/receiving data through DMA). After commenting out the
line of code that puts the device in EM2, this example used about 1.8 milliamps
on average. Note: this energy measurement was done using Simplicity Studio's
built-in energy profiler for a GG11 device with a Debug build configuration and
no optimization flags (gcc -O0).

================================================================================

For further help, see the DMA/LDMA section in the Reference Manual. One could
also look at the DMA and LDMA Application Notes.

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit.
2. Use a USB to serial device (such as the CP210x). Connect the CP210x's RX pin
   to the device's TX pin. Connect the CP210x's TX pin to the device's RX pin.
   These port and pin mappings are listed below.
3. Open up a serial terminal device such as Termite.
4. In Termite, open the port connected to the CP210x device (check which port
   using Device Manager).
5. After typing in Termite and pressing enter, the input will be echoed back

================================================================================

Peripherals Used:
LFXO - 32.768 kHz (reference clock for the LFB clock branch)
LEUART0 - 9600 baud, 8-N-1 (8 data bits, no parity, one stop bit)

================================================================================

Listed below are the port and pin mappings for working with this example. Unless
explicity specified otherwise, the pin corresponds to the pin on the expansion
headers.

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD4 - LEUART0 TX (Pin 12)
PD5 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD4 - LEUART0 TX (Pin 12)
PD5 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32G Starter Kit (GXXX-STK)
Device: EFM32G890F128
PD4 - LEUART0 TX (Pin 12)
PD5 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD4 - LEUART0 TX (Pin 12)
PD5 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD4 - LEUART0 TX (Pin 12)
PD5 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD4 - LEUART0 TX (Pin 12)
PD5 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PD4 - LEUART0 TX (Pin 12)
PD5 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PD10 - LEUART0 TX (Pin 12)
PD11 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A)
Device: EFM32MG1P232F256GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFR32MG12P Radio Board (BRD4161A)
Device: EFR32MG12P432F1024GL125
PA0 - LEUART0 TX (top middle row of breakout pads, Pin 33)
PA1 - LEUART0 RX (top middle row of breakout pads, Pin 34)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A)
Device: EFM32FG1P133F256GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32FG12P Starter Kit (BRD4253A)
Device: EFM32FG12P433F1024GL125
PA0 - LEUART0 TX (top middle row of breakout pads, Pin 33)
PA1 - LEUART0 RX (top middle row of breakout pads, Pin 34)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A)
Device: EFM32BG1P232F256GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32BG12P Starter Kit (BRD4103A)
Device: EFM32BG12P332F1024GL125
PA0 - LEUART0 TX (top middle row of breakout pads, Pin 33)
PA1 - LEUART0 RX (top middle row of breakout pads, Pin 34)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFR32BG14 Starter Kit (BRD4105A)
Device: EFR32BG14P732F256GM48
PA0 - LEUART0 TX (Pin 12)
PA1 - LEUART0 RX (Pin 14)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC14 - LEUART0 TX (top right row of breakout pads, Pin 25)
PC15 - LEUART0 RX (top right row of breakout pads, Pin 27)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC14 - LEUART0 TX (Pin 12)
PC15 - LEUART0 RX (Pin 14)


