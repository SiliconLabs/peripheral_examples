/***************************************************************************//**
 * @file main_xg21.c
 * @brief This project demonstrates a simple analog comparison of push button 1
 * (PD03) to the 1.25V internal VREF; if the button is pushed, this pulls the
 * input voltage to GND, and the comparator output low. The MCU core is in sleep
 * mode in EM3, but the analog comparator remains operating. In this project,
 * the analog comparator generates an edge interrupt that wakes the MCU, and a
 * GPIO EXP_HEADER14 (PA06) is toggled in the interrupt handler before returning
 * to EM3.
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
 
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_acmp.h"
#include "em_burtc.h"
#include "bsp.h"

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure push button and GPIO output
  GPIO_PinModeSet(gpioPortD, 3, gpioModeInput, 1);
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief ACMP initialization
 *****************************************************************************/
void initACMP(void)
{
  // Initialize with default settings
  ACMP_Init_TypeDef init = ACMP_INIT_DEFAULT;
  ACMP_Init(ACMP0, &init);

  // Allocate CDODD0 to ACMP0 to be able to use the input
  GPIO->CDBUSALLOC = GPIO_CDBUSALLOC_CDODD0_ACMP0;
  
  // In this example we want to compare an analog input to the 1.25 V
  // internal reference. The default settings resets the divider for
  // acmpInputVREFDIV1V25, which we can use as a 1.25 V reference.
  // Now we select the two inputs to compare. Here we compare the acmpInputPD3
  // input to the internal 1.25V reference. When acmpInputPD3 is lower than
  // 1.25 V then the ACMP output is 0 and when acmpInputPD3 is higher than
  // 1.25 V then the ACMP output is 1.
  ACMP_ChannelSet(ACMP0, acmpInputVREFDIV1V25, acmpInputPD3);

  // Wait for warm-up
  while (!(ACMP0->IF & ACMP_IF_ACMPRDY));

  // Clear pending ACMP interrupts
  NVIC_ClearPendingIRQ(ACMP0_IRQn);
  ACMP_IntClear(ACMP0, ACMP_IF_RISE | ACMP_IF_FALL);

  // Enable ACMP interrupts
  NVIC_EnableIRQ(ACMP0_IRQn);
  ACMP_IntEnable(ACMP0, ACMP_IEN_RISE | ACMP_IEN_FALL);
}

/**************************************************************************//**
 * @brief  ACMP Handler
 *****************************************************************************/
void ACMP0_IRQHandler(void)
{
  if(ACMP0->IF & ACMP_IF_RISE)
  {
	// Clear interrupt flag
    ACMP_IntClear(ACMP0, ACMP_IF_RISE);

    // Turn on GPIO
    GPIO_PinOutSet(gpioPortA, 6);
  }
  if(ACMP0->IF & ACMP_IF_FALL)
  {
	// Clear interrupt flag
    ACMP_IntClear(ACMP0, ACMP_IF_FALL);

    // Turn off GPIO
    GPIO_PinOutClear(gpioPortA, 6);
  }
}

/***************************************************************************//**
 * @brief   Disable high frequency clocks
 ******************************************************************************/
static void disableHFClocks(void)
{
  // Make sure all high frequency peripherals are disabled
  USART0->EN_CLR = 0x1;
  USART1->EN_CLR = 0x1;
  USART2->EN_CLR = 0x1;
  TIMER0->EN_CLR = 0x1;
  TIMER1->EN_CLR = 0x1;
  TIMER2->EN_CLR = 0x1;
  TIMER3->EN_CLR = 0x1;
  ACMP1->EN_CLR = 0x1;
  IADC0->EN_CLR = 0x1;
  I2C0->EN_CLR = 0x1;
  I2C1->EN_CLR = 0x1;
  GPCRC->EN_CLR = 0x1;

  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);

  // Check that HFRCODPLL and HFXO are not requested
  while (((HFRCO0->STATUS & _HFRCO_STATUS_ENS_MASK) != 0U)
         || ((HFXO0->STATUS & _HFXO_STATUS_ENS_MASK) != 0U));
}

/***************************************************************************//**
 * @brief   Disable low frequency clocks
 ******************************************************************************/
static void disableLFClocks(void)
{
  // Make sure all low frequency peripherals are disabled
  RTCC->EN_CLR = 0x1;
  WDOG0->EN_CLR = 0x1;
  WDOG1->EN_CLR = 0x1;
  LETIMER0->EN_CLR = 0x1;
  BURTC->EN_CLR = 0x1;

  // Check that all low frequency oscillators are stopped
  while ((LFRCO->STATUS != 0U) && (LFXO->STATUS != 0U));
}

/***************************************************************************//**
 * @brief   Disable all clocks to achieve lowest current consumption numbers.
 ******************************************************************************/
static void disableClocks(void)
{
  // Disable High Frequency Clocks
  disableHFClocks();

  // Disable Low Frequency Clocks
  disableLFClocks();
}

/***************************************************************************//**
 * @brief
 *   Enter EM3 with BURTC running on ULFRCO.
 *
 * @param[in] powerdownRam
 *   Power down all RAM except the first 16 kB block or retain full RAM.
 *
 * @details
 *   Parameter:
 *     EM3. Stop Mode.@n
 *   Condition:
 *     BURTC, Full RAM, 1kHz ULFRCO.@n
 *
 * @note
 *   To better understand disabling clocks and oscillators for specific modes,
 *   see Reference Manual section EMU-Energy Management Unit and Table 9.2.
 ******************************************************************************/
void em_EM3_UlfrcoBURTC(bool powerdownRam)
{
  // Make sure clocks are disabled
  disableClocks();

  // Select ULFRCO as the BURTC clock source
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);

  // Setup BURTC
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  BURTC_Init(&burtcInit);

  // Power down all RAM blocks except block 1
  if (powerdownRam) {
    EMU_RamPowerDown(SRAM_BASE, 0);
  }

  // Enter EM3
  EMU_EnterEM3(false);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Use default settings for EM23 and HFXO
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_WSTK_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Initializations
  initGPIO();
  initACMP();

  // Initialize EM2/EM3 with default parameters
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  // Infinite loop
  while(1)
  {
    // Enter EM3 with BURTC running from ULFRCO clock - current consumption
	// mode defined in DS
    em_EM3_UlfrcoBURTC(true);
  }
}
