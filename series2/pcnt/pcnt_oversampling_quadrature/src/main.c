/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the oversampling quadrature decoder mode of
 * the pulse counter peripheral. The program will request an interrupt whenever
 * the quadrature decoder detects a change in direction or a counter overflow or
 * counter underflow. The example utilizes OVSQUAD 4X mode, such that counter
 * value changes with each state transition of S1IN/S0IN (see PCNT State
 * Transitions in Reference Manual). LED0 toggles on detected direction changes,
 * and LED1 toggles on counter overflow/underflow events. Optional pulse width
 * filter and flutter removal are both enabled.
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

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_pcnt.h"
#include "em_prs.h"
#include "mx25flash_spi.h"
#include "bsp.h"

// PRS channels to use for GPIO/PCNT
#define PCNT_PRS_Ch0    0
#define PCNT_PRS_Ch1    1

// PCNT Top value with hysteresis
#define PCNTTopValue    10   // Interrupt every 6 S1IN/S0IN state transitions in
                             //   the same direction

// Push-buttons are active-low
#define PB_PRESSED      0

/*
 * This example enters EM2 in the infinite while loop; Setting this define to 1
 * enables debug connectivity in the EMU_CTRL register, which will consume about
 * 0.5uA additional supply current; comment this line of code to disable
 */
#define EM2DEBUG        1

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

int32_t absCnt = 0;         // Counter value valid upon interrupt completion
uint32_t UFCnt = 0;
uint32_t OFCnt = 0;

/***************************************************************************//**
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
 ******************************************************************************/
void escapeHatch(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN,
                  gpioModeInputPullFilter, 1);
  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0) {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN,
                    gpioModePushPull, 1);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else {
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN,
                    gpioModeDisabled, 0);
    CMU_ClockEnable(cmuClock_GPIO, false);
  }
}

/***************************************************************************//**
 * @brief PCNT0 interrupt handler
 *        This function acknowledges the interrupt and toggles LED0/LED1
 ******************************************************************************/        
void PCNT0_IRQHandler(void)
{
  uint32_t flags = PCNT_IntGet(PCNT0);

  PCNT_IntClear(PCNT0, (PCNT_IF_DIRCNG | PCNT_IF_OF | PCNT_IF_UF));

  if(PCNT0->STATUS & PCNT_STATUS_DIR) {  // check direction; indicate with LED0
    GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);  // set LED0 if down
  } else {
    GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN); // clear LED0 if up
  }

  if(flags & PCNT_IF_OF) {                // check for overflow
    GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN); // toggle led 1
    OFCnt += 1;
  }
  else if(flags & PCNT_IF_UF) {           // check for underflow
    GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN); // toggle led 1
    UFCnt += 1;
  }

  /*
   * absCnt value with hysteresis
   * Only valid on interrupt; CNT can/will change between interrupts
   * and not be captured in absCnt value until next direction change or
   * overflow/underflow event
   */
  absCnt = PCNT0->CNT - UFCnt * (PCNTTopValue/2 + 1) +
      OFCnt * (PCNTTopValue/2 + 1);
}

/***************************************************************************//**
 * @brief Initialize PCNT0
 *        This function sets up PCNT0 with oversampling quadrature mode
 *        S0 PRS linked to PRSCH0
 *        S1 PRS linked to PRSCH1
 ******************************************************************************/
static void initPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;
  PCNT_Filter_TypeDef pcntFilterInit = PCNT_FILTER_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);

  pcntInit.mode     = pcntModeOvsQuad4; // OVS quadrature mode
  pcntInit.top      = PCNTTopValue;
  pcntInit.s1CntDir = false;            // S1 does not affect counter direction,
                                        // using default init setting; count up
  pcntInit.s0PRS    = PCNT_PRS_Ch0;
  pcntInit.s1PRS    = PCNT_PRS_Ch1;
  pcntInit.filter   = true;
  pcntInit.cntEvent = pcntCntEventBoth; // count up and down
  pcntInit.hyst     = true;

  // Use max filter len for GPIO push button
  pcntFilterInit.filtLen = _PCNT_OVSCTRL_FILTLEN_MASK;
  pcntFilterInit.flutterrm = true;

  /*
   * With flutter removal enabled, counter is only updated if the current and
   * previous state transition of the rotation are in the same direction.
   */

  // Enable PRS0 input
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  // Enable PRS1 input
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS1, true);

  // Filter configuration
  PCNT_FilterConfiguration(PCNT0, &pcntFilterInit, true);

  // PCNT0 Init
  PCNT_Init(PCNT0, &pcntInit);

  /*
   * On-board push button is active low on WSTK and PCNT is configured to count
   * on rising edge. Upon enabling PCNT module, initial state of push button
   * will cause the counter to increment without a button press.
   *
   * Wait here until this count propagates through counter-clock domain.
   */
  while(PCNT0->CNT == 0);

  /*
   * Reset the count back to zero.
   *
   * Alternatively, modifying the PCNT initialization configuration to count on
   * falling edge (add "pcntInit.negEdge = true;" before PCNT_Init() function
   * call) would also alleviate additional count on PCNT module start-up, or the
   * PRS logic could be modified to negate the discrete on-board logic (select
   * NOT A function logic in PRS channel's FNSEL field). For this example, the
   * desire is to increment counter on push button release. Note that negating
   * both (negEdge and PRS logic) results in the same problem and would require
   * the counter clear that has been implemented for accurate count of the first
   * overflow event.
   */
  PCNT0->CMD_SET = PCNT_CMD_CNTRST;

  // Enable PCNT0 interrupt
  PCNT_IntEnable(PCNT0, (PCNT_IEN_OF | PCNT_IEN_UF | PCNT_IEN_DIRCNG));

  // Clear PCNT0 pending interrupt
  NVIC_ClearPendingIRQ(PCNT0_IRQn);

  // Enable PCNT0 interrupt in the interrupt controller
  NVIC_EnableIRQ(PCNT0_IRQn);
}

/***************************************************************************//**
 * @brief Initialize PRS
 *        This function sets up GPIO PRS which links BTN0 and BTN1 to PRS0 and
 *        PRS1, respectively
 ******************************************************************************/        
static void initPrs(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  // Set up GPIO PRS
  PRS_SourceAsyncSignalSet(PCNT_PRS_Ch0, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           BSP_GPIO_PB0_PIN);

  // Set up GPIO PRS
  PRS_SourceAsyncSignalSet(PCNT_PRS_Ch1, PRS_ASYNC_CH_CTRL_SOURCESEL_GPIO,
                           BSP_GPIO_PB1_PIN);
}

/***************************************************************************//**
 * @brief Initialize GPIO setup
 *        This function configures BTN0 and BTN1 as input and enables
 *        external interrupts for BTN0 and BTN1
 ******************************************************************************/        
static void initGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize LED driver
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

  // Configure pin I/O - BTN0
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN,
                  gpioModeInputPullFilter, 1);

  // Configure pin I/O - BTN1
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN,
                  gpioModeInputPullFilter, 1);

  // Configure BTN0 for external interrupt
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN,
                    false, false, false);

  // Configure BTN1 for external interrupt
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN,
                    false, false, false);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  FlashStatus status;

  CHIP_Init();

  // Recommended recovery procedure for code in development
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

  // Use LFRCO as clock for EM23GRPACLK and PCNT
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFRCO);

#ifdef EM2DEBUG
  // Enable debug connectivity in EM2
  EMU->CTRL_SET = EMU_CTRL_EM2DBGEN;
#endif

  // Initialize GPIO
  initGpio();

  // Initialize and power-down MX25 SPI flash
  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();

  // PRS Initialization
  initPrs();

  // PCNT Initialization
  initPcnt();

  /*
   * Enter EM2 forever; PCNT/EM23GRACLK sourced from LFRCO - only available down
   * to EM2. EM23GRACLK can be sourced by the slower ULFRCO to run in EM3,
   * however, maximum input toggle frequency should be 2 times slower, and is
   * further limited by the (optional; enabled in this example) pulse width
   * filter.
   */
  while (true) {
    EMU_EnterEM2(false);
  }
}
