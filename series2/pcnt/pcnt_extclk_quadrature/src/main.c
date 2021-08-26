/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the external quadrature mode of the pulse
 * counter peripheral. The program will request an interrupt whenever the
 * quadrature decoder detects a change in direction.
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
#define PCNT_PRS_Ch0     0
#define PCNT_PRS_Ch1     1

// Push-buttons are active-low
#define PB_PRESSED      0

/*
 * This example enters EM2 in the infinite while loop; Setting this define to 1
 * enables debug connectivity in the EMU_CTRL register, which will consume about
 * 0.5uA additional supply current; comment this line of code to disable
 */
#define EM2DEBUG        1

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
 *        This function acknowledges the interrupt and toggles LED0
 ******************************************************************************/        
void PCNT0_IRQHandler(void)
{
  PCNT_IntClear(PCNT0, PCNT_IF_DIRCNG);
  if(PCNT0->STATUS & PCNT_STATUS_DIR)	{  // check direction for toggling LEDs
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);	// LED0 if down
  } else {
    GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);	// LED1 if up
  }
}

/***************************************************************************//**
 * @brief Initialize PCNT0
 *        This function sets up PCNT0 with external quadrature mode
 *        S0 PRS linked to PRSCH0
 *        S1 PRS linked to PRSCH1
 ******************************************************************************/
static void initPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);

  pcntInit.mode     = pcntModeDisable;  // Initialize with PCNT disabled
  pcntInit.s1CntDir = false;            // S1 does not affect counter direction,
                                        // using default init setting; count up
  pcntInit.s0PRS    = PCNT_PRS_Ch0;
  pcntInit.s1PRS    = PCNT_PRS_Ch1;

  // Enable PRS0 input
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  // Enable PRS1 input
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS1, true);

  // PCNT0 Init
  PCNT_Init(PCNT0, &pcntInit);

  // Set mode to externally clocked quadrature decoder
  PCNT_Enable(PCNT0, pcntModeExtQuad);

  // Change to the external clock
  CMU->PCNT0CLKCTRL = CMU_PCNT0CLKCTRL_CLKSEL_PCNTS0;

  /*
   * When the PCNT operates in externally clocked mode and switching external
   * clock source, a few clock pulses are required on the external clock to
   * synchronize accesses to the externally clocked domain. This example uses
   * push button PB0 via GPIO/PRS/PCNT0_S0IN for the external clock, which would
   * require multiple button presses to sync the PCNT registers and clock
   * domain, during which button presses would not be counted.
   *
   * To get around this, such that each button push is recognized, firmware
   * can use the PRS software pulse triggering mechanism to generate
   * those first few pulses. This allows the first actual button press and
   * subsequent button presses to be properly counted.
   */
  while (PCNT0->SYNCBUSY) {
    PRS_PulseTrigger(1 << PCNT_PRS_Ch0);
  }

  // Enable PCNT0 interrupt
  PCNT_IntEnable(PCNT0, PCNT_IEN_DIRCNG);

  // Clear PCNT0 pending interrupt
  NVIC_ClearPendingIRQ(PCNT0_IRQn);

  // Enable PCNT0 interrupt in the interrupt controller
  NVIC_EnableIRQ(PCNT0_IRQn);
}

/***************************************************************************//**
 * @brief Initialize PRS
 *        This function sets up GPIO PRS which links BTN0 to PCNT0 PRS0
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
 * @brief Initialize GPIO
 *        This function initializes push button PB0 and PB1 and enables external
 *        interrupts for PRS functionality
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

  // Enter EM3 forever
  while (true) {
    EMU_EnterEM3(false);
  }
}
