gpio_em4_wakeup

This project uses GPIO to wake the device from EM4 mode and thus trigger a
reset. Under non-EM4 reset, the device will enter EM4. Pressing PB0 (on
EFR32xG21)/ PB1 (on EFR32xG22, EFR32xG23, EFR32xG24, EFR32xG25, EFR32xG27,
EFR32xG28) will wake the device from EM4 and cause the LEDs on the STK/Radio
board to toggle indefinitely. PB0 on EFR32xG22, EFR32xG23, EFR32xG24, EFR32xG25,
and EFR32xG28 is used as an "escape hatch". This is a way to pause the device so
that a debugger can connect in order to erase flash, among other things. Before
proceeding with this example, make sure PB0 is NOT pressed. GPIO PC03 is used as
an escape hatch on EFR32xG27. Before proceeding with this example, make sure
PC03 is not low.

Note for EFR32xG21 devices, clock enabling is not required.

How To Test:
1. Build the project and download it to the Starter Kit
2. Close debug session in IDE
3. Observe the current consumption of the device which indicates that device is
   in EM4. Use the Energy Profiler to measure current consumption. 
   Connect the device directly to the Energy Profiler and observe the current
   consumption. 
   Alternatively, while capturing current consumption, set the debug mode to 
   OFF by 
   a) selecting the Jlink device in debug adapters and,
   b) opening Device Configuration. 
   c) Under Device Hardware, enter the appropriate Target Part (shown below 
   under Device)
   Observe the current consumption by connecting to the board after this. 
4. Press PB0 (on EFR32xG21)/PB1 (on EFR32xG22/EFR32xG23/EFR32xG24/EFR32xG25/
   EFR32xG27/EFR32xG28) to exit EM4.
5. Observe the LEDs blinking, indicating that the last reset cause was exit 
   from EM4. Capture current consumption which implies device is in EM0.

Note for Testing on xG28:
On BRD4400C, there is a diversity SPDT switch on the 2.4 GHz radio output that
will consume ~63 uA of additional current when supplied power via logic high on
GPIO PD02. The pin is configured in disabled mode (high-Z) by default, thus SPDT
switch is powered down by default. Be advised that when PD02 is driven to logic
high, observed current consumption will be higher than what is specified in the 
EFR32ZG28 device datasheet due to this additional integrated circuit.

Peripherals Used:
CMU    - HFRCODPLL @ 19 MHz
EMU
RMU
USART  - used only to power down onboard SPI flash

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 -  Red LED on radio board, yellow LED0 on mainboard
PD02 -  Push button PB0

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB00 -  Push button PB0 (Escape hatch)
PB01 -  Push button PB1
PD02 -  LED0
PD03 -  LED1

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4204D) + 
        Wireless Starter Kit Mainboard
Device: EFR32ZG23B010F512IM48
PB01 -  Push Button PB0
PB03 -  Push Button PB1
PB02 -  LED0
PD03 -  LED1

Board:  Silicon Labs EFR32xG24 Radio Board (BRD4186C) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG24B210F1536IM48
PB01 -  Push Button PB0
PB03 -  Push Button PB1
PB02 -  LED0
PB04 -  LED1

Board:  Silicon Labs EFR32xG25 Radio Board (BRD4270B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG25B222F1920IM56
PB00 -  Push Button PB0
PB01 -  Push Button PB1
PC06 -  LED0
PC07 -  LED1

Board:  Silicon Labs EFR32xG27 Radio Board (BRD4194A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG27C140F768IM40
PB01 -  Push Button PB1
PB00 -  LED0
PC03 -  Escape Hatch

Board:  Silicon Labs EFR32xG28 Radio Board (BRD4400C) +
        Wireless Starter Kit Mainboard
Device: EFR32ZG28B312F1024IM68
PB01 -  Push Button PB0
PB03 -  Push Button PB1
PB02 -  LED0
PD03 -  LED1