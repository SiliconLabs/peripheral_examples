/***************************************************************************//**
 * @file main_xg25.c
 * @brief This project demonstrates the tamper detection module for EFR32xG25.
 * The example uses #defines to enable the ETAMPDET peripheral's channel 0
 * and/or channel 1, and requires an external jumper-wire connection between the
 * ETAMPIN0 <-> ETAMPOUT0 and/or ETAMPIN1 <-> ETAMPOUT1 pins, as specified
 * in device datasheet and README. The application is configured similarly to
 * the em4_no_dcdc example, allowing EM4 entry with BURTC with LFXO _or_ EM4
 * entry without BURTC utilizing ULFRCO via #define at the beginning of code.
 * ETAMPDET is enabled for EM4 wakeup. If MCU reset is caused by EM4 wakeup and
 * ETAMPDET interrupt flag is set, LED0 or LED1 will toggle to indicate the
 * ETAMPDET tamper event on channel 0 or channel 1, respectively.
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
 
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_burtc.h"
#include "em_core.h"
#include "em_rmu.h"
#include "bsp.h"
#include "mx25flash_spi.h"

volatile uint32_t msTicks; // counts 1ms timeTicks

/*
 *  Define to enable BURTC running from LFXO in EM4; set to 1 to enable, set to
 *  0 for example to enter EM4 without BURTC (ETAMPDET will run from ULFRCO)
 */
#define BURTC_LFXO_EN 0

#ifdef _SILICON_LABS_32B_SERIES_2_CONFIG_7
#define LED_OUT 0
#else
#define LED_OUT 1
#endif

/*
 * These defines can be used to enable/disable each of ETAMPDET peripherals
 * two tamper detection channels
 */
#define ETAMP_CH0_EN 1
#define ETAMP_CH1_EN 1

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       // increment counter necessary in Delay()
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/***************************************************************************//**
 * @brief
 *    Powers down the SPI flash on the radio board
 *
 * @details
 *    A JEDEC standard SPI flash boots up in standby mode in order to
 *    provide immediate access, such as when used it as a boot memory.
 *
 *    Typical current draw in standby mode for the MX25R8035F device used
 *    on EFR32 radio boards is 5 µA.
 *
 *    JEDEC standard SPI flash memories have a lower current deep power-down
 *    mode, which can be entered after sending the relevant commands. This is on
 *    the order of 0.007 µA for the MX25R8035F.
 ******************************************************************************/
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
 * @brief  GPIO Initializer
 *****************************************************************************/
void initGPIO(void)
{
  // Enable GPIO clock branch
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure LEDs as outputs; will toggle for tamper detection
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, !LED_OUT);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, !LED_OUT);
}

/***************************************************************************//**
 * @brief LFXO initialization
 ******************************************************************************/
void initLFXO(void)
{
  CMU_LFXOInit_TypeDef lfxoInit = BSP_LFXOINIT_DEFAULT;
  CMU_LFXOInit(&lfxoInit);
}

/***************************************************************************//**
 * @brief ETAMPDET initialization
 ******************************************************************************/
void initETAMPDET(void)
{
  uint8_t chnl0_en, chnl1_en;
  chnl0_en = 0;
  chnl1_en = 0;

#ifdef ETAMP_CH0_EN
#if (ETAMP_CH0_EN == 1)
  chnl0_en = 1;
#endif
#endif

#ifdef ETAMP_CH1_EN
#if (ETAMP_CH1_EN == 1)
  chnl1_en = 1;
#endif
#endif

  // Enable register clock; ETAMPDET clock not currently defined in em_cmu.h
  CMU->CLKEN1_SET = CMU_CLKEN1_ETAMPDET;

  if (chnl0_en == 1) { // Configure GPIO for ETAMPDET channel 0
    // Disable GPIO signals associated with used ETAMPER channel 0
    GPIO_PinModeSet(gpioPortA, 5, gpioModeDisabled, 0);
    GPIO_PinModeSet(gpioPortA, 6, gpioModeDisabled, 0);
  }

  if (chnl1_en == 1) { // Configure GPIO for ETAMPDET channel 1
    // Disable GPIO signals associated with used ETAMPER channel 1
    GPIO_PinModeSet(gpioPortD, 5, gpioModeDisabled, 0);
    GPIO_PinModeSet(gpioPortD, 4, gpioModeDisabled, 0);
  }

  // Make sure module is disabled before configuring
  ETAMPDET->EN_CLR = ETAMPDET_EN_EN;

  /*
   *  Must wait for peripheral to disable before modifying other registers; if
   *  register write is attempted before peripheral is disabled, hard fault will
   *  occur.
   */
  while(ETAMPDET->EN && ETAMPDET_EN_DISABLING);

  // Configure upper and lower prescaler values
  ETAMPDET->CLKPRESCVAL = ETAMPDET_CLKPRESCVAL_LOWERPRESC_DivideBy64 |
                          ETAMPDET_CLKPRESCVAL_UPPERPRESC_Bypass;

  // Load a random seed value for channel 0/1
  ETAMPDET->CHNLSEEDVAL0 = 0x167DC55F;
  ETAMPDET->CHNLSEEDVAL1 = 0x5F04B84A;

  // Enable ETAMPDET as EM4 wakeup source
  ETAMPDET->EM4WUEN_SET = (chnl0_en << _ETAMPDET_EM4WUEN_CHNLEM4WUEN0_SHIFT) |
      (chnl1_en << _ETAMPDET_EM4WUEN_CHNLEM4WUEN1_SHIFT);

  /*
   *   Additional ETAMPDET configurable settings; configuring for both channels
   *   regardless of channel enablement
   */
  ETAMPDET->CFG_SET = ETAMPDET_CFG_CHNLTAMPDETFILTEN0 | ETAMPDET_CFG_CHNLTAMPDETFILTEN1;
  ETAMPDET->CNTMISMATCHMAX =
      ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX0_DetectFilterThreshold4 |
      ETAMPDET_CNTMISMATCHMAX_CHNLCNTMISMATCHMAX1_DetectFilterThreshold4;
  ETAMPDET->CHNLFILTWINSIZE =
      ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE0_DetectFilterMovingWinSize10 |
      ETAMPDET_CHNLFILTWINSIZE_CHNLFILTWINSIZE1_DetectFilterMovingWinSize10;

  // Channel 0/1 pad enable
  ETAMPDET->CFG_SET = (chnl0_en << _ETAMPDET_CFG_CHNLPADEN0_SHIFT) |
      (chnl1_en << _ETAMPDET_CFG_CHNLPADEN1_SHIFT);

  // Enable ETAMPDET module
  if (chnl0_en || chnl1_en)
    ETAMPDET->EN_SET = ETAMPDET_EN_EN;

  // Load channel 0/1 linear feedback shift register (LFSR) seeds
  ETAMPDET->CMD_SET = (chnl0_en << _ETAMPDET_CMD_CHNLLOAD0_SHIFT) |
      (chnl1_en << _ETAMPDET_CMD_CHNLLOAD1_SHIFT);

  /*
   *  Command register writes must synchronize with the LF clock
   *  Check the SYNCBUSY register for busy status; wait for register write
   */
  while (ETAMPDET->SYNCBUSY != 0);

  // Start channel 0/1 LFSR
  ETAMPDET->CMD_SET = (chnl0_en << _ETAMPDET_CMD_CHNLSTART0_SHIFT) |
      (chnl1_en << _ETAMPDET_CMD_CHNLSTART1_SHIFT);

  // Wait for register write
  while (ETAMPDET->SYNCBUSY != 0);
}

/***************************************************************************//**
 * @brief
 *   Enter EM4 with BURTC running on a LFXO
 *
 * @details
 *   Parameter:
 *     EM4H. Hibernate Mode.@n
 *   Condition:
 *     BURTC, 128 byte RAM, 32.768 kHz LFXO.@n
 *
 ******************************************************************************/
void em_EM4_LfxoBURTC(void)
{
  // Setup BURTC.
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  CMU_ClockEnable(cmuClock_BURTC, true);
  BURTC_Init(&burtcInit);

  // Enter EM4
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.retainLfxo = true;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
}

/***************************************************************************//**
 * @brief
 *   Enter EM4 without BURTC.
 *
 * @details
 *   Parameter:
 *     EM4.@n
 *   Condition:
 *     No BURTC, 128 byte BURAM.@n
 *
 ******************************************************************************/
void em_EM4(void)
{
  // Enter EM4
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.pinRetentionMode = emuPinRetentionLatch;
  EMU_EM4Init(&em4Init);
  EMU_EnterEM4();
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
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);
  if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) == 0) {
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, LED_OUT);
    __BKPT(0);
  }
  // Pin not asserted, so disable input
  else {
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, (!LED_OUT));
    CMU_ClockEnable(cmuClock_GPIO, false);
  }
}

/***************************************************************************//**
 * @brief Main function
 ******************************************************************************/
int main(void)
{
  uint32_t cause;
  uint8_t tamperDetectedCh0, tamperDetectedCh1;
  tamperDetectedCh0 = 0;
  tamperDetectedCh1 = 0;

  // Chip errata
  CHIP_Init();

  EMU_UnlatchPinRetention();
  cause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  /*
   *  If PB0 is depressed during MCU reset, prevents EM2 entry allowing
   *  debug/reprogram of device
   */
  escapeHatch();

  // Initialize GPIO for LEDs
  initGPIO();

  if ((cause & EMU_RSTCAUSE_EM4)) {
    // Setup SysTick Timer for 1 msec interrupts
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
      while (1);

    // Enable register clock; ETAMPDET clock not currently defined in em_cmu.h
    CMU->CLKEN1_SET = CMU_CLKEN1_ETAMPDET;

    // Set the appropriate channel flag for LED indicator
    if ((ETAMPDET->IF & _ETAMPDET_IF_TAMPDET0_MASK) == ETAMPDET_IF_TAMPDET0)
      tamperDetectedCh0 = 1;
    else if ((ETAMPDET->IF & _ETAMPDET_IF_TAMPDET1_MASK) == ETAMPDET_IF_TAMPDET1)
      tamperDetectedCh1 = 1;

    // Disable ETAMPDET module; no longer needed in typical applications
    ETAMPDET->EN_CLR = ETAMPDET_EN_EN;

    /*
     *  Register writes to ETAMPDET peripheral cannot occur until disable
     *  completes
     */
    while(ETAMPDET->EN && ETAMPDET_EN_DISABLING);

    ETAMPDET->IEN_CLR = _ETAMPDET_IEN_MASK;

    ETAMPDET->IF_CLR = _ETAMPDET_IF_MASK;

    // Infinite while loop; toggle LED based on tamper detect channel
    while(tamperDetectedCh0 | tamperDetectedCh1) {
        if (tamperDetectedCh0 == 1)
          GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
        else if (tamperDetectedCh1 == 1)
          GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);

        Delay(500);
    }
  }

  // Enable DC-DC converter
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Power down the SPI flash
  powerDownSpiFlash();

  // Route desired oscillator to shared BURTC/ETAMPDET clock tree
  if (BURTC_LFXO_EN) {
    // Initialize the LFXO
    initLFXO();

    // Select LFXO as clock source.
    CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFXO);
  } else {
    // Select ULFRCO as clock source.
    CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  }

  // Initialize ETAMPDET peripheral
  initETAMPDET();

  // Enter EM4
  if(BURTC_LFXO_EN)
    em_EM4_LfxoBURTC();
  else
    em_EM4();

  // Should never reach here unless something goes awry entering EM4
  while (1);
}
