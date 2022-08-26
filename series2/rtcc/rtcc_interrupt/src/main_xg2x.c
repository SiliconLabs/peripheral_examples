/***************************************************************************//**
 * @file main_xg2x.c
 * @brief This project demonstrates use of the RTCC to wake from EM2/3. See
 * readme.txt for details.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "em_prs.h"
#include "em_rtcc.h"
#include "mx25flash_spi.h"
#include "bsp.h"

#define PRS_Output_Port         gpioPortB
#define PRS_Output_Pin          0

/**************************************************************************//**
 * The defines below select the RTCC clock source, the PRS output channel and
 * the wake up interval from EM2/3.
 * If the RTCC Clock source is defined as LFXO, the device will go into EM2. To
 * test the example in EM3, the RTCC Clock source must be changed to
 * cmuSelect_ULFRCO.
 *****************************************************************************/

#define RTCC_CLOCK              cmuSelect_LFXO    // RTCC clock source
#define RTCC_PRS_CH             0                 // RTCC PRS output channel
#define WAKEUP_INTERVAL_MS      500               // Wake up interval in ms

/**************************************************************************//**
 * A JEDEC standard SPI flash boots up in standby mode in order to
 * provide immediate access, such as when used it as a boot memory.
 *
 * Typical current draw in standby mode for the MX25R8035F device used
 * on EFR32 radio boards is 5 uA.
 *
 * JEDEC standard SPI flash memories have a lower current deep power-down mode,
 * which can be entered after sending the relevant commands.  This is on the
 * order of 0.007 uA for the MX25R8035F.
 *****************************************************************************/
void powerDownSpiFlash(void)
{
  FlashStatus status;

  MX25_init();
  MX25_RSTEN();
  MX25_RST(&status);
  MX25_DP();
  MX25_deinit();
}

/**************************************************************************//**
 * @brief  
 *   RTCC Interrupt Handler, clears the flag.
 *****************************************************************************/
void RTCC_IRQHandler(void)
{
  // Clear interrupt source CC1
  RTCC_IntClear(RTCC_IF_CC1);
}

/**************************************************************************//**
 * @brief  
 *   Setup RTCC with selected clock source. The clock source can be modified by
 *   changing the definition of RTCC_CLOCK.
 * @param[in] rtccClock
 *   Select clock source, valid values are cmuSelect_LFRCO, cmuSelect_LFXO, and
 *   cmuSelect_ULFRCO.
 *****************************************************************************/
void setupRtcc(CMU_Select_TypeDef rtccClock)
{
  // Configure the RTCC with default parameters
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  RTCC_CCChConf_TypeDef rtccInitCompareChannel = RTCC_CH_INIT_COMPARE_DEFAULT;

  // Check RTCC clock source
  if (rtccClock == cmuSelect_LFXO) {
    // Initialize LFXO with specific parameters
    CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
    CMU_LFXOInit(&lfxoInit);
  }

  // Setting RTCC clock source
  CMU_ClockSelectSet(cmuClock_RTCCCLK, rtccClock);

  // Enable RTCC bus clock
  CMU_ClockEnable(cmuClock_RTCC, true);

  // Initialize Capture Compare Channel 1 to toggle PRS output on compare match
  rtccInitCompareChannel.compMatchOutAction = rtccCompMatchOutActionToggle;
  RTCC_ChannelInit(1, &rtccInitCompareChannel);

  /**********************************************************************//**
   * Set RTCC Capture Compare Value to the frequency of the
   * LFXO/ULFRCO(minus 1).
   *
   * When the pre-counter (RTCC_PRECNT) matches this value, the counter
   * (RTCC_CNT) increments by one.
   *************************************************************************/
  if (rtccClock == cmuSelect_ULFRCO) {
    //Setting the CC1 compare value of the RTCC
    RTCC_ChannelCCVSet(1, (1000 * WAKEUP_INTERVAL_MS) / 1000 - 1);
  } else {
    //Setting the CC1 compare value of the RTCC
    RTCC_ChannelCCVSet(1, (32768 * WAKEUP_INTERVAL_MS) / 1000 - 1);
  }

  // Initialize the RTCC
  rtccInit.cntWrapOnCCV1 = true;        // Clear counter on CC1 compare match
  rtccInit.presc = rtccCntPresc_1;      // Prescaler 1

  // Initialize and start counting
  RTCC_Init(&rtccInit);

  // Enabling Interrupt from RTCC CC1
  RTCC_IntEnable(RTCC_IEN_CC1);
  NVIC_ClearPendingIRQ(RTCC_IRQn);
  NVIC_EnableIRQ(RTCC_IRQn);
}

/**************************************************************************//**
 * @brief  
 *   Main function
 *****************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Turn on DCDC regulator
  EMU_DCDCInit(&dcdcInit);

  // Enable voltage downscaling in EM modes 2 and 3
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;

  // Initialize EM modes 2 and 3
  EMU_EM23Init(&em23Init);

  // Power-down the radio board SPI flash
  powerDownSpiFlash();

  /**********************************************************************//**
   * When developing/debugging code on xG22 that enters EM2 or lower,
   * it's a good idea to have an "escape hatch" type mechanism, e.g. a
   * way to pause the device so that a debugger can connect in order
   * to erase flash, among other things.
   *
   * Before proceeding with this example, make sure PB1 is not pressed.
   * If the PB1 pin is low, turn on LED0 and execute the breakpoint
   * instruction to stop the processor in EM0 and allow a debug
   * connection to be made.
   *************************************************************************/
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);

  if (GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN) == 0)
  {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else
  {
    GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeDisabled, 0);
  }

  // Enable peripheral clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Setting up RTCC
  setupRtcc(RTCC_CLOCK);

  /**********************************************************************//**
   * RTCC CC1 PRS output on LED1. While all GPIO pins retain their state in
   * EM2, only pins on port A and B remain fully functional in EM2/3.
   *
   * In this example, pin PB0 is used as the output of the PRS channel. This
   * pin is routed to the expansion header on the WSTK (EXP Pin 7).
   *************************************************************************/
  GPIO_PinModeSet(PRS_Output_Port, PRS_Output_Pin, gpioModePushPull, 1);
  PRS_SourceAsyncSignalSet(RTCC_PRS_CH, PRS_ASYNC_CH_CTRL_SOURCESEL_RTCC,
                           PRS_ASYNC_CH_CTRL_SIGSEL_RTCCCCV1);
  PRS_PinOutput(RTCC_PRS_CH, prsTypeAsync, PRS_Output_Port, PRS_Output_Pin);

  /**********************************************************************//**
   * Power down first 24 KB to reduce current in EM2/3.
   *
   * NOTE: The top 8 KB of RAM (BLK1) **MUST** remain powered in EM2/3
   * (SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL != 2) or the device is liable to
   * hard fault on wake-up depending on what data may have been saved
   * on the stack, e.g. the return address from the EMU_EnterEM3() call
   * below.
   *************************************************************************/
  EMU_RamPowerDown(SRAM_BASE, SRAM_BASE + SRAM_SIZE - 0x2000);

  while (1) {
    // Forever enter EM2 or EM3 depending on RTCC clock source
    if (CMU_ClockSelectGet(cmuClock_RTCCCLK) == cmuSelect_ULFRCO) {
      EMU_EnterEM3(false);
    } else {
      EMU_EnterEM2(false);
    }
  }
}
