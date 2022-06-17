/***************************************************************************//**
 * @file
 * @brief This example uses LESENSE to scan two ACMP channel in low energy mode.
 *        The LESENSE is able to detect when the input signal crosses over a
 *        certain threshold and will update the decoder states based on 
 *        different input
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/
#include <stdint.h>
#include "em_device.h"
#include "em_acmp.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_core.h"
#include "em_lesense.h"
#include "bspconfig.h"
#include "bsp.h"
#include "mx25flash_spi.h"

#define PD01REGNORETAIN   0  // EM0/1 peripheral register retention
#define EM2_DEBUG         1  // EM2 debug enable
#define SCAN_FREQ         8  // 8 Hz
#define STATE_LED         1  // state transition LED demo, turn off to view
                             // state transition current consumption
#define NO_TURN           0
#define LEFT_TURN         1
#define RIGHT_TURN        2
#define BOTH_TURN         3

// Determine the BBUSALLOC to allocate to ACMP0
#if (BSP_GPIO_PB0_PIN % 2 == 0 && BSP_GPIO_PB1_PIN % 2 == 0)
  #define GPIO_BBUSALLOC   GPIO_BBUSALLOC_BEVEN0_ACMP0
#elif (BSP_GPIO_PB0_PIN % 2 != BSP_GPIO_PB1_PIN % 2)
  #define GPIO_BBUSALLOC   GPIO_BBUSALLOC_BEVEN0_ACMP0 | GPIO_BBUSALLOC_BODD0_ACMP0
#else
  #define GPIO_BBUSALLOC   GPIO_BBUSALLOC_BODD0_ACMP0
#endif

uint32_t decoder_state;

/**************************************************************************//**
 * @brief LESENSE interrupt handler
 *        This function acknowledges the interrupt and reads the current
 *        decoder state
 ******************************************************************************/
void LESENSE_IRQHandler(void)
{
  // Clear all LESENSE interrupt flag
  uint32_t flags = LESENSE_IntGet();
  LESENSE_IntClear(flags);

  if(flags & LESENSE_IF_DEC){
    // Check current decoder state
    decoder_state = LESENSE_DecoderStateGet();
  }
}

/**************************************************************************//**
 * @brief escapeHatch()
 * When developing or debugging code that enters EM2 or
 * lower, it's a good idea to have an "escape hatch" type
 * mechanism, e.g. a way to pause the device so that a debugger can
 * connect in order to erase flash, among other things.
 *
 * Before proceeding with this example, make sure PB0 is not pressed.
 * If the PB0 pin is low, turn on LED0 and execute the breakpoint
 * instruction to stop the processor in EM0 and allow a debug
 * connection to be made.
 *****************************************************************************/
void escapeHatch(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN,
                  gpioModeInputPullFilter, 1);
  if (GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN) == 0) {
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else {
    GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeDisabled, 0);
    CMU_ClockEnable(cmuClock_GPIO, false);
  }
}

/**************************************************************************//**
 * @brief ACMP Initialization
 *        This function configures and enables ACMP0 for usage by LESENSE
 ******************************************************************************/
void initACMP(void)
{
  CMU_ClockEnable(cmuClock_ACMP0, true);

  // ACMP Configuration
  ACMP_Init_TypeDef initACMP = ACMP_INIT_DEFAULT;

  // Initialize ACMP
  ACMP_Init(ACMP0, &initACMP);

  // Allocate BODD0 to ACMP0 to be able to use the input
  GPIO->BBUSALLOC_SET = GPIO_BBUSALLOC;

  // Select 1.25V internal as the reference voltage for ACMP negative input
  ACMP0->INPUTCTRL_SET = ACMP_INPUTCTRL_NEGSEL_VREFDIV1V25;

  // Port B external interface override
  ACMP0->INPUTCTRL_SET = ACMP_INPUTCTRL_POSSEL_EXTPB;

  // Wait for warm-up
  while (!(ACMP0->STATUS && ACMP_IF_ACMPRDY));
}

/**************************************************************************//**
 * @brief GPIO initialization
 *        This function configures LED0 and LED1 as output
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

#if STATE_LED
  // Configure LED0 and LED1 for output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);
#endif
}

/**************************************************************************//**
 * @brief LESENSE Initialization
 *        This functions configures and enables the LESENSE block
 *****************************************************************************/
void initLESENSE(void)
{
  /*****************************************************************************
   * Set LFRCO as EFM32GRPACLK clock source
   * Enable clock for LESENSE
   * Enable PRS clock as it is needed for LESENSE initialization
  *****************************************************************************/
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_LESENSE, true);
  CMU_ClockEnable(cmuClock_PRS, true);

  // LESENSE default configurations
  LESENSE_Init_TypeDef initLesense = LESENSE_INIT_DEFAULT;
  LESENSE_ChDesc_TypeDef initLesenseCh = LESENSE_CH_CONF_DEFAULT;
  LESENSE_DecCtrlDesc_TypeDef initDecode = LESENSE_DECCTRL_DESC_DEFAULT;

  initDecode.hystIRQ = false;   // disable interrupt based hysteresis
  initDecode.intMap  = false;   // Do not set interrupt on channel

  // invert ACMP0 to accommodate the PB0 and PB1 pull-up
  initLesense.coreCtrl.storeScanRes = true;
  initLesense.coreCtrl.invACMP0 = 1;

  // Enable LESENSE control of the ACMP0 positive input mux
  initLesense.perCtrl.acmp0Mode = lesenseACMPModeMux;
  initLesense.decCtrl = initDecode;

  // Channel Configuration
  initLesenseCh.enaScanCh   = true;  // Enable scan channel
  initLesenseCh.sampleDelay = 0x1;   // 1+1 LF Clock cycle sample delay
  initLesenseCh.sampleMode  = lesenseSampleModeACMP;
  initLesenseCh.shiftRes    = true;  // shift result to decoder register

  //Initialize LESENSE interface
  LESENSE_Init(&initLesense, true);

  /****************************************************************************
    * @description: Configure LESENSE decoder states
    * There are a total of 2 push buttons to be scanned so a total of four
    * states are needed. The initial starting state is state 0, and state
    * transition will happen when the compare value matches with the sensorState
    * result on the non-masked channel.
    ***************************************************************************/
  LESENSE_DecStAll_TypeDef initStAll = LESENSE_DECODER_CONF_DEFAULT;

  initStAll.St[0].curState  = 0;     // state 0 = no_turn
  initStAll.St[0].nextState = 0;     // Set next state to itself
  initStAll.St[0].compMask  = 0xC;   // enable both channels
  initStAll.St[0].compVal   = 0x00;  // no button pressed

  initStAll.St[1].curState  = 0;     // state 0 = no_turn
  initStAll.St[1].nextState = 1;     // Set next state to 1 = left_turn
  initStAll.St[1].compMask  = 0xC;   // enable both channels
  initStAll.St[1].compVal   = 0x01;  // compare match on PB0 detect
  initStAll.St[1].setInt    = true;  // enable interrupt upon match

  initStAll.St[2].curState  = 0;     // state 0 = no_turn
  initStAll.St[2].nextState = 2;     // Set next state to 2 = right_turn
  initStAll.St[2].compMask  = 0xC;   // enable both channels
  initStAll.St[2].compVal   = 0x02;  // compare match on PB1 detect
  initStAll.St[2].setInt    = true;  // enable interrupt upon match

  initStAll.St[3].curState  = 0;     // state 0 = no_turn
  initStAll.St[3].nextState = 3;     // Set next state to 3 = both_turn
  initStAll.St[3].compMask  = 0xC;   // enable both channels
  initStAll.St[3].compVal   = 0x03;  // compare match on both buttons
  initStAll.St[3].setInt    = true;  // enable interrupt upon match

  initStAll.St[4].curState  = 1;     // state 1 = left_turn
  initStAll.St[4].nextState = 0;     // Set next state to state 0 = no_turn
  initStAll.St[4].compMask  = 0xC;   // enable both channels
  initStAll.St[4].compVal   = 0x00;  // if no press, go back to state 0
  initStAll.St[4].setInt    = true;  // enable interrupt upon match

  initStAll.St[5].curState  = 1;     // state 1 = left_turn
  initStAll.St[5].nextState = 1;     // Set next state to itself
  initStAll.St[5].compMask  = 0xC;   // enable both channels
  initStAll.St[5].compVal   = 0x01;  // compare match on PB0

  initStAll.St[6].curState  = 1;     // state 1 = left_turn
  initStAll.St[6].nextState = 2;     // Set next state to state 2 = right_turn
  initStAll.St[6].compMask  = 0xC;   // enable both channels
  initStAll.St[6].compVal   = 0x02;  // compare match on PB1
  initStAll.St[6].setInt    = true;  // enable interrupt upon match

  initStAll.St[7].curState  = 1;     // state 1 = left_turn
  initStAll.St[7].nextState = 3;     // Set next state to state 3 = both_turn
  initStAll.St[7].compMask  = 0xC;   // enable both channels
  initStAll.St[7].compVal   = 0x03;  // compare match on both channel
  initStAll.St[7].setInt    = true;  // enable interrupt upon match

  initStAll.St[8].curState  = 3;     // state 3 = both_turn
  initStAll.St[8].nextState = 3;     // Set next state to itself
  initStAll.St[8].compMask  = 0xC;   // enable both channels
  initStAll.St[8].compVal   = 0x03;  // if both pressed, stay in state 3

  initStAll.St[9].curState  = 3;     // state 3 = both_turn
  initStAll.St[9].nextState = 1;     // Set next state to state 1 = left_turn
  initStAll.St[9].compMask  = 0xC;   // enable both channels
  initStAll.St[9].compVal   = 0x01;  // compare match on PB0
  initStAll.St[9].setInt    = true;  // enable interrupt upon match

  initStAll.St[10].curState  = 3;     // state 3 = both_turn
  initStAll.St[10].nextState = 2;     // Set next state to state 2 = right_turn
  initStAll.St[10].compMask  = 0xC;   // enable both channels
  initStAll.St[10].compVal   = 0x02;  // compare match on PB1
  initStAll.St[10].setInt    = true;  // enable interrupt upon match

  initStAll.St[11].curState  = 3;     // state 3 = both_turn
  initStAll.St[11].nextState = 0;     // Set next state to state 0 = no_turn
  initStAll.St[11].compMask  = 0xC;   // enable both channels
  initStAll.St[11].compVal   = 0x00;  // if none pressed, go to state 0
  initStAll.St[11].setInt    = true;  // enable interrupt upon match

  initStAll.St[12].curState  = 2;     // state 2 = right_turn
  initStAll.St[12].nextState = 0;     // Set next state to state 0 = no_turn
  initStAll.St[12].compMask  = 0xC;   // enable both channels
  initStAll.St[12].compVal   = 0x00;  // if none pressed, go to state 0
  initStAll.St[12].setInt    = true;  // enable interrupt upon match

  initStAll.St[13].curState  = 2;     // state 2 = right_turn
  initStAll.St[13].nextState = 1;     // Set next state to state 1 = left_turn
  initStAll.St[13].compMask  = 0xC;   // enable both channels
  initStAll.St[13].compVal   = 0x01;  // compare match on PB0
  initStAll.St[13].setInt    = true;  // enable interrupt upon match

  initStAll.St[14].curState  = 2;     // state 2 = right_turn
  initStAll.St[14].nextState = 2;     // Set next state to itself
  initStAll.St[14].compMask  = 0xC;   // enable both channels
  initStAll.St[14].compVal   = 0x02;  // compare match on PB1

  initStAll.St[15].curState  = 2;     // state 2 = right_turn
  initStAll.St[15].nextState = 3;     // Set next state to state 3 = both_turn
  initStAll.St[15].compMask  = 0xC;   // enable both channels
  initStAll.St[15].compVal   = 0x03;  // compare match on both buttons
  initStAll.St[15].setInt    = true;  // enable interrupt upon match

  LESENSE_DecoderStateAllConfig(&initStAll); // Initialize decoder

  // Configure channel 0 and channel 1
  LESENSE_ChannelConfig(&initLesenseCh, 0);
  LESENSE_ChannelConfig(&initLesenseCh, 1);

  // 8 Hz scan
  LESENSE_ScanFreqSet(0, SCAN_FREQ);

  // Wait for SYNCBUSY clear
  while(LESENSE->SYNCBUSY);

  // Disable LESENSE, needed in order to configure the offset and internal timer
  LESENSE->EN_CLR = LESENSE_EN_EN;
  while (LESENSE->EN & _LESENSE_EN_DISABLING_MASK);

  // LESENSE offset is set to the ACMP input pin numbers
  LESENSE->CH_SET[0].INTERACT = (BSP_GPIO_PB0_PIN << _LESENSE_CH_INTERACT_OFFSET_SHIFT);
  LESENSE->CH_SET[1].INTERACT = (BSP_GPIO_PB1_PIN << _LESENSE_CH_INTERACT_OFFSET_SHIFT);

  // Enable LESENSE
  LESENSE->EN_SET = LESENSE_EN_EN;
  while(LESENSE -> SYNCBUSY);         // SYNCBUSY check;

  // Enable decoder interrupt and interrupt in NVIC
  LESENSE_IntEnable(LESENSE_IEN_DEC);
  NVIC_ClearPendingIRQ(LESENSE_IRQn);
  NVIC_EnableIRQ(LESENSE_IRQn);

  // Disable PRS clock as it is no longer needed
  CMU_ClockEnable(cmuClock_PRS, false);

  // Start continuous scan
  LESENSE_ScanStart();
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  FlashStatus status;

  // Chip errata
  CHIP_Init();

  // EM2 escapeHatch to re-enable debug control
  escapeHatch();

  // Select FSRCO for SYSCLK; FSRCO is default out of reset
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);

  // Turn on DCDC regulator
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_WSTK_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Enable voltage downscaling in EM2/3 (VSCALE0)
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;

  // Initialize EM mode 2/3
  EMU_EM23Init(&em23Init);

/*****************************************************************************
 * The reference manual section 13.3.6 states that the EM0 / EM1 peripheral
 * register retention can be optionally disabled if the application does not
 * need to retain the EM0 / EM1 peripheral register setting upon low energy mode
 * wake up. This can be done by setting bit 0 in the EMU_PD1PARETCTRL register.
 *
 * Note this will cause the the EM0 / EM1 peripheral register interface to reset
 * upon exit to EM0 and will need to be reconfigured. This example does not use
 * any peripherals that operates in EM0 / EM1 only, and therefore has this bit
 * configured for lower current consumption.
*****************************************************************************/
#if PD01REGNORETAIN
  EMU->PD1PARETCTRL_SET = 0x1;
#endif

#if EM2_DEBUG
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;  // Enable EM2 debug
#endif

  // Initialize GPIO
  initGPIO();
  
  // Initialize and power-down MX25 SPI flash
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  // Initialize ACMP
  initACMP();
  
  // Initialize LESENSE
  initLESENSE();

  while(1) {
    EMU_EnterEM2(false);        //put system into EM2 mode

#if STATE_LED
    switch(decoder_state) {
      // state 0, both LED off
      case NO_TURN:
        GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
        GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
      break;

      // state 1, LED0 on, LED1 off
      case LEFT_TURN:
        GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
        GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
      break;

      // state 2, LED0 off, LED1 on
      case RIGHT_TURN:
        GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
        GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
      break;

      // state 3, both LED on
      case BOTH_TURN:
        GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
        GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
      break;

      // default state
      default:
      break;
    }
#endif
  }
}
