lesense_single_channel

This project demonstrates the usage of LESENSE to sample a single channel
using ACMP on the EFR32FG23 device. The LESENSE uses the ACMP peripheral to
sample the input state of push button 0, and will trigger an interrupt to
toggle LED0 when push button 0 is pressed.

Use Simplicity Studio's Energy Profiler to observe current consumption while
example runs in low energy mode.

Note: In project where the device enters EM2 or lower, an escapeHatch
      routine is usually recommended to prevent device lock-up. This example has
      implemented a escapeHatch, where if the user holds down push-button 1 when
      resetting the device, the escapeHatch will hold the device in EM0 state
      to allow debug connection

How to test:
1. Build the project and download it to the starter kit.
2. The LED0 should be off right now with a current consumption of around 3.8uA.
3. Press push button 0, LED0 should turn on.
4. Release push button 0 and press it again, LED0 should turn off.
5. Enter debug mode and place a break point inside the LESENSE_IRQHandler().
6. Repeat step 3 and observe the interrupt handler firing each time.

Peripheral Used:
LFRCO - 32768 Hz
ACMP  - used to sample push-button 0 input state
GPIO  - LED0 configured as output
LESENSE - controls ACMP to sample push-button 0 on selected LESENSE channel

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB01 - Push Button PB0
PB02 - LED0
PB03 - Push Button PB1


