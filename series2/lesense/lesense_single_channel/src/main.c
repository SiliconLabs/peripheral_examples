/***************************************************************************//**
 * @file
 * @brief This example uses LESENSE to scan one ACMP channel in low energy mode.
 *         The LESENSE is able to detect when the input signal crosses over a
 *         certain threshold and will trigger an interrupt to toggle the LED.
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
#include "em_usart.h"
#include "bspconfig.h"
#include "bsp.h"
#include "mx25flash_spi.h"

#define PD01REGNORETAIN  0  // EM0/1 peripheral register retention
#define EM2_DEBUG        1  // EM2 debug enable
#define SCAN_FREQ        8  // 8 Hz

// Determine the BBUSALLOC to allocate to ACMP0
#if (BSP_GPIO_PB0_PIN % 2 == 0)
  #define GPIO_BBUSALLOC   GPIO_BBUSALLOC_BEVEN0_ACMP0
#else
  #define GPIO_BBUSALLOC   GPIO_BBUSALLOC_BODD0_ACMP0
#endif

/**************************************************************************//**
 * @brief LESENSE interrupt handler
 *        This function acknowledges the interrupt and toggles LED0.
 ******************************************************************************/
void LESENSE_IRQHandler(void)
{
  // Clear all LESENSE interrupt flag
  uint32_t flags = LESENSE_IntGet();
  LESENSE_IntClear(flags);

  if (flags & LESENSE_IF_CH0){
    // Toggles LED0
    GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
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
 *        This function configures LED0 as push-pull output
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LED0 for output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
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

  // Do not store scan result and invert ACMP0 to accommodate the PB0 pull-up
  initLesense.coreCtrl.storeScanRes = false;
  initLesense.coreCtrl.invACMP0 = 1;
  
  // Enable LESENSE control of the ACMP0 positive input mux
  initLesense.perCtrl.acmp0Mode = lesenseACMPModeMux;

  // Channel Configuration
  initLesenseCh.enaScanCh = true;  // Enable scan channel
  initLesenseCh.enaInt = true;     // Enable interrupt
  initLesenseCh.sampleDelay = 0x1; // 1+1 LF Clock cycle sample delay
  initLesenseCh.sampleMode = lesenseSampleModeACMP;
  initLesenseCh.intMode = lesenseSetIntPosEdge;  // Interrupt on sensor posEdge
  initLesenseCh.offset = BSP_GPIO_PB0_PIN; //Set LESENSE offset

  //Initialize LESENSE interface
  LESENSE_Init(&initLesense, true);

  // Configure channel 0
  LESENSE_ChannelConfig(&initLesenseCh, 0);

  // 8 Hz scan
  LESENSE_ScanFreqSet(0, SCAN_FREQ);

  // Wait for SYNCBUSY clear
  while(LESENSE->SYNCBUSY);

  LESENSE->EN_SET = LESENSE_EN_EN;  // Enable LESENSE
  while(LESENSE->SYNCBUSY);         // SYNCBUSY check;

  // Enable interrupt in NVIC
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
  }
}
