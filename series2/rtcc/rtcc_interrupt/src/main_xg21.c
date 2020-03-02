/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the functions of RTCC.
 *        See readme.txt for details.
 * @version 0.0.1
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_prs.h"
#include "em_rtcc.h"
#include "bsp.h"

// Defines
#define RTCC_CLOCK              cmuSelect_LFXO  // RTCC clock source
#define RTCC_PRS_CH             0               // RTCC PRS output channel
#define WAKEUP_INTERVAL_MS      500             // Wake up interval in ms

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
 *   Setup RTCC with selected clock source
 * @param[in] rtccClock
 *   Select clock source, valid values are cmuSelect_LFRCO, cmuSelect_LFXO, and
 *   cmuSelect_ULFRCO.
 *****************************************************************************/
void setupRtcc(CMU_Select_TypeDef rtccClock)
{
  RTCC_Init_TypeDef rtccInit = RTCC_INIT_DEFAULT;
  RTCC_CCChConf_TypeDef rtccInitCompareChannel = RTCC_CH_INIT_COMPARE_DEFAULT;

  if (rtccClock == cmuSelect_LFXO) {
    // Initialize LFXO with specific parameters
    CMU_LFXOInit_TypeDef lfxoInit = CMU_LFXOINIT_DEFAULT;
    CMU_LFXOInit(&lfxoInit);
  }

  // Setting RTCC clock source
  CMU_ClockSelectSet(cmuClock_RTCCCLK, rtccClock);

  // Initialize CC1 to toggle PRS output on compare match
  rtccInitCompareChannel.compMatchOutAction = rtccCompMatchOutActionToggle;
  RTCC_ChannelInit(1, &rtccInitCompareChannel);

  // Setting the CC1 compare value of the RTCC
  if (rtccClock == cmuSelect_ULFRCO) {
    RTCC_ChannelCCVSet(1, (1000 * WAKEUP_INTERVAL_MS) / 1000 - 1);
  } else {
    RTCC_ChannelCCVSet(1, (32768 * WAKEUP_INTERVAL_MS) / 1000 - 1);
  }

  // Initialize the RTCC
  rtccInit.cntWrapOnCCV1 = true;        // Clear counter on CC1 compare match
  rtccInit.presc = rtccCntPresc_1;      // Prescaler 1
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
  // Chip errata
  CHIP_Init();

  // Setting up RTCC
  setupRtcc(RTCC_CLOCK);

  // RTCC CC1 PRS output on LED1 (PA or PB to work on EM2 or EM3)
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1);
  PRS_SourceAsyncSignalSet(RTCC_PRS_CH, PRS_ASYNC_CH_CTRL_SOURCESEL_RTCC,
                           PRS_ASYNC_CH_CTRL_SIGSEL_RTCCCCV1);
  PRS_PinOutput(RTCC_PRS_CH, prsTypeAsync, BSP_GPIO_LED1_PORT,
                BSP_GPIO_LED1_PIN);

  while (1) {
    // Forever enter EM2 or EM3
    if (CMU_ClockSelectGet(cmuClock_RTCCCLK) == cmuSelect_ULFRCO) {
      EMU_EnterEM3(false);
    } else {
      EMU_EnterEM2(false);
    }
  }
}
