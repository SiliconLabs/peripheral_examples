gpio_em4_wakeup

This project uses GPIO to wake the device from EM4 mode and thus trigger a 
reset.
Under non-EM4 reset, the device will enter EM4. Pressing PB0 (on EFR32xG21)/
PB1 (on EFR32xG22) will wake the device from EM4 and cause the LEDs on the 
STK/Radio board to toggle indefinitely. PB0 on EFR32xG22 is used as an "escape
hatch". This is a way to pause the device so that a debugger can connect in 
order to erase flash, among other things. Before proceeding with this example, 
make sure PB0 is not pressed.

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
5. Press PB0 (on EFR32xG21)/PB1 (on EFR32xG22) to exit EM4.
4. Observe the LEDs blinking, indicating that the last reset cause was exit 
   from EM4. Capture current consumption which implies device is in EM0.

Peripherals Used:
FSRCO  - 20 MHz

Board:  Silicon Labs EFR32xG21 Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG21A010F1024IM32
PB00 - Red LED on radio board, yellow LED0 on mainboard
PD02 - Push button PB0

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PB00 - Push button PB0 (Escape hatch)
PB01 - Push button PB1
PD02 - LED0
PD03 - LED1