lesense_decoder

This project demonstrates the usage of LESENSE to sample two channels
using ACMP on the EFR32FG23 device. The LESENSE uses the ACMP peripheral to
sample the input state of push button 0 and 1. The dedicated LESENSE decoder is
used to read the sensor state and update its internal state machine according to
the user configuration. Upon valid transition, the LESENSE interrupt will fire
and read its current state to update the LEDs.

Total states in the state machine: 4
Total arcs used: 16
See doc/state_machine.svg for the state machine diagram.

For this example, the LESENSE internal state machine is configured to represent
a traffic turning control system. The LED0 will signal a valid left turn and
LED1 will signal a valid right turn. The push buttons represent whether it is
safe to make the turn. If PB0 is pressed, left turn is safe to make. If PB1 is
pressed, right turn is safe to make. If both buttons pressed, it is safe to make
both turns.

state 0: no turns should be make (both buttons not pressed)
state 1: left turn safe to make (PB0 pressed, PB1 not pressed, LED0 on)
state 2: right turn safe to make (PB0 not pressed, PB1 pressed, LED1 on)
state 3: both turn safe to make (PB0/1 pressed, LED0/1 on)

The different arcs represent all possible inputs that can cause a state
transition. For example, from state 0 you can:
1. Transition to state 0 by pressing no button (input 00)
2. Transition to state 1 by pressing PB0 (input 01)
3. Transition to state 2 by pressing PB1 (input 10)
4. Transition to state 3 by pressing PB0 and PB1 (input 11)

Each state can have four different transitions or arcs, therefore a total of 16
arcs are used here. Also note that this is a Moore state machine, so the output
will only depend on the state, not the input.

Use Simplicity Studio's Energy Profiler to observe current consumption while
example runs in low energy mode.

Note: In project where the device enters EM2 or lower, an escapeHatch
      routine is usually recommended to prevent device lock-up. This example has
      implemented a escapeHatch, where if the user holds down push-button 1 when
      resetting the device, the escapeHatch will hold the device in EM0 state
      to allow debug connection

How to test:
1. Build the project and enter debug mode
2. Place a breakpoint within the LESENSE_IRQHandler()
3. Add decoder_state as a watch expression variable
4. Run the project, press PB0, the LESENSE_IRQHandler should trigger
5. Step through the line decoder_state = LESENSE_DecoderStateGet()
6. Observe the decoder_state which indicates the current state
7. Continue running the example and try different push button configuration
8. Observe the decoder_state variable

To observe current consumption,
1. In main_xG23.c, set EM2_DEBUG to 0
2. Build the project and use Energy Profiler to observe current consumption
3. Current consumption should be around 3.8uA

Peripheral Used:
LFRCO - 32768 Hz
ACMP  - used to sample push-button 0 and push-button 1 input state
GPIO  - LED0 and LED1 configured as push-pull output
LESENSE - controls ACMP to sample push-button 0/1 on selected LESENSE channel

Board:  Silicon Labs EFR32xG23 Radio Board (BRD4263B) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG23A010F512GM48
PB01 - Push Button PB0
PB03 - Push Button PB1
PB02 - LED0
PD03 - LED1


